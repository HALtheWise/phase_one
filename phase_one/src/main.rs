use std::{io::Stdin, thread, time::Duration};

use esp_idf_svc::hal::{
    ledc::{config::TimerConfig, LedcDriver, LedcTimerDriver, Resolution},
    prelude::Peripherals,
    units::FromValueType as _,
};

fn main() -> anyhow::Result<()> {
    // It is necessary to call this function once. Otherwise some patches to the runtime
    // implemented by esp-idf-sys might not link properly. See https://github.com/esp-rs/esp-idf-template/issues/71
    esp_idf_svc::sys::link_patches();

    // Bind the log crate to the ESP Logging facilities
    esp_idf_svc::log::EspLogger::initialize_default();

    log::info!("Hello, world!");

    let frequency = 200.Hz();
    let frequency_us = 1_000_000 / frequency.0;
    let min_period_us = 600;
    // let min_period_us = 0;
    let max_period_us = 2_300;

    let peripherals = Peripherals::take()?;
    let timer = LedcTimerDriver::new(
        peripherals.ledc.timer0,
        &TimerConfig::default()
            .frequency(frequency)
            .resolution(Resolution::Bits12),
    )?;

    let mut pwms = [
        LedcDriver::new(peripherals.ledc.channel0, &timer, peripherals.pins.gpio4)?,
        LedcDriver::new(peripherals.ledc.channel1, &timer, peripherals.pins.gpio5)?,
        LedcDriver::new(peripherals.ledc.channel2, &timer, peripherals.pins.gpio6)?,
        LedcDriver::new(peripherals.ledc.channel3, &timer, peripherals.pins.gpio7)?,
        LedcDriver::new(peripherals.ledc.channel4, &timer, peripherals.pins.gpio15)?,
        LedcDriver::new(peripherals.ledc.channel5, &timer, peripherals.pins.gpio16)?,
    ];

    let max = pwms[0].get_max_duty();
    let mut set_all = |us: u32| -> anyhow::Result<()> {
        for pwm in &mut pwms {
            pwm.set_duty(max * us / frequency_us)?;
        }
        Ok(())
    };

    let mut us = min_period_us;
    set_all(us)?;
    thread::sleep(Duration::from_millis(500));
    while us < max_period_us {
        set_all(us)?;
        us += (max_period_us - min_period_us) / 300;
        thread::sleep(Duration::from_millis(10));
    }

    while us > min_period_us {
        us -= (max_period_us - min_period_us) / 300;
        set_all(us)?;
        thread::sleep(Duration::from_millis(10));
    }

    set_all(0)?;

    Ok(())
}
