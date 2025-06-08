MarIOnette Serial Protocol Documentation

The MarIOnette system uses a binary serial protocol to communicate between a control application (like Blender) and the Arduino-compatible board. The protocol consists of single-character commands, some of which are followed by a data payload.

### General Packet Structure

For commands that send data, the general packet structure is:

`<COMMAND> <LENGTH_H> <LENGTH_L> <DATA...> <TERMINATOR>`

-   `COMMAND`: A single ASCII character that defines the action to be taken.
-   `LENGTH_H`, `LENGTH_L`: Two bytes representing a 16-bit unsigned integer (`word(LENGTH_H, LENGTH_L)`). The meaning of this length value is command-specific.
-   `DATA...`: A sequence of data bytes. The structure and length of this data are specific to each command.
-   `TERMINATOR`: A single `.` character (ASCII 46) to signify the end of the packet.

A small delay (`SERIAL_DELAY`) is required between sending each byte to ensure reliable communication, especially with AVR-based boards.

---

### Commands

#### 'A': Update Motors (with Bus Servo Speed)

This command updates the positions of all configured motors and includes a speed setting for bus servos.

-   **Structure**: `<A> <LENGTH_H> <LENGTH_L> <SPEED_H> <SPEED_L> <MOTOR_DATA...> <.>`
-   **Length Field**: The `word(LENGTH_H, LENGTH_L)` value specifies the size of the `<MOTOR_DATA...>` portion in bytes. It should be equal to `TOTAL_MOTORS * 2`.
-   **Data Fields**:
    -   `SPEED`: 2 bytes (16-bit unsigned integer) for the speed of bus servos.
    -   `MOTOR_DATA`: A sequence of bytes representing motor positions. Each position is a 2-byte value. The total length is `TOTAL_MOTORS * 2` bytes.

**Example** (`TOTAL_MOTORS = 2`):
`<A> <0x00> <0x04> <speed_H> <speed_L> <motor1_H> <motor1_L> <motor2_H> <motor2_L> <.>`
*In this example, the length field is 4, which is the length of the motor data (`<motor1_H> <motor1_L> <motor2_H> <motor2_L>`). The total data payload between length and terminator is 6 bytes (2 for speed + 4 for motors).*

---

#### 'B': Update Motors (without Bus Servo Speed)

This command updates the positions of all configured motors, without a speed setting.

-   **Structure**: `<B> <LENGTH_H> <LENGTH_L> <MOTOR_DATA...> <.>`
-   **Length Field**: The `word(LENGTH_H, LENGTH_L)` value specifies the size of `<MOTOR_DATA...>` in bytes. It should be equal to `TOTAL_MOTORS * 2`.
-   **Data Fields**:
    -   `MOTOR_DATA`: `TOTAL_MOTORS * 2` bytes. Each motor's position is a 2-byte value.

**Example** (`TOTAL_MOTORS = 2`):
`<B> <0x00> <0x04> <motor1_H> <motor1_L> <motor2_H> <motor2_L> <.>`

---

#### 'P': Play Animation from SD Card

Starts playing a pre-recorded animation file from the SD card. `SD_ENABLE` must be set to `1` in `config.h`.

-   **Packet**: Just the character `'P'`.

---

#### 'S': Stop Animation Playback

Stops the currently playing SD card animation.

-   **Packet**: Just the character `'S'`.

---

#### 'R': Record Motor Positions (Work in Progress)

Intended to start recording motor positions to create an animation. This feature is currently under development.

-   **Packet**: Just the character `'R'`.

---

#### 'Z': Cancel Recording

Stops the recording process initiated by the `'R'` command.

-   **Packet**: Just the character `'Z'`. 