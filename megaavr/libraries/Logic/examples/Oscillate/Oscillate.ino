/***********************************************************************|
  | megaAVR Configurable Custom Logic library                             |
  |                                                                       |
  | Oscillate.ino                                                         |
  |                                                                       |
  | A library for interfacing with the megaAVR Configurable Custom Logic. |
  | Developed in 2019 by MCUdude. https://github.com/MCUdude/             |
  | Example by Spence Konde, 2020 https://github.com/SpenceKonde/         |
  |                                                                       |
  | In this example, we demonstrate the fact that these are ASYNCHRONOUS  |
  | by (ab)using the CCL in a configuration which will oscillate - this   |
  | example uses two logic blocks - though there are plenty of ways to    |
  | make it oscillate with a single one (see if you can come up with some |
  | - assuming you find this fun). If you set values for the filter, you  |
  | get sane output frequencies - just what one would expect from the     |
  | datasheet. But what fun is that?! If you turn them both off, it       |
  | oscillates much faster - faster than the clock speed! with a sawtooth |
  | waveform which (measured by my 'scope) clocks in at 37 MHz - from a   |
  | part running at 20 MHz! Different configurations will result in       |
  | different frequencies. The simplest one can oscillate at a whopping   |
  | 92 MHz!                                                               |
  |                                                                       |
  | It is temperature dependenr - point a can of freeze spray (computer   |
  | duster held upside-down, so the liquid comes out) and the frequency   |
  | goes up. Aim a heat-gun or torch at it (use care - freeze spray DOES  |
  | burn, and you do NOT want to inhale the combustion products or cause  |
  | an inferno in your lab.) and the frequency goes down. Warnings:       |
  | Hitting hot parts with freeze spray may crack the package, ruining the|
  | part, and freeze spray will cause frost to form on the parts. Best to |
  | disconnect them from power while they thaw and dry off before applying|
  | power to them again. PCBs also burn if you are not careful with that  |
  | torch. Freeze spray, despite the statements some carry to the contrary|
  | absolutely is flammable, and the combustion products aren't pleasant. |
  | Freeze-sprays contain highly potent greenhouse gasses. For these      |
  | reasons I do not recommend attempting to duplicate that part of the   |
  | test. This is maximum-speed oscillation is of essentially zero        |
  | practical use, but it sure is cool isn't it?                          |
  |                                                                       |
  | Maybe it could be used to examine a new silicon revision to see if    |
  | there were any significant process changes? *shrug*                   |
  |                                                                       |
  | And yes, if you happen to want your part to run at an indeterminate   |
  | and highly temperature dependent speed, you could then set the system |
  | clock prescaler to 2, connect a jumper between the Logic0 output pin  |
  | and EXTCLK pin, and switch to the external clock, leading to it       |
  | running at around 18.5 MHz with wide variation depending on conditions|
  | This is not recommended except as a silly joke, and it is thoroughly  |
  | useless. Maybe if you miss the +/- 10% tolerance on the classic AVR   |
  | internal oscillator?                                                  |
  |                                                                       |
  | In combination with the synchronizer/filter, though, it is has the    |
  | potential to be far more useful - as it will allow generation of a    |
  | prescaled system clock. On parts where a TCB can be clocked from an   |
  | event, this allows one to work around the limited prescale options    |
  | available for TCBs, without having to change the TCA prescaler. For   |
  | more information on this, see: https://github.com/SpenceKonde/        |
  | AVR-Guidance/blob/master/CCL_EVSYS_hacks/CCL_prescaling.md            |
  |                                                                       |
  |***********************************************************************/

#include <Logic.h>
#include <Event.h>


void setup() {
  Serial.begin(115200);
}

void demo1() {
  /* Async Demo 1: Just how fast is this "asynchronous" stuff?
   *
   * First, let's do the most obvious case - enable a single input, set it as feedback, and tell the logic block to turn on when the input is LOW and off when input is HIGH. If using a 'scope, monitor the output pin, and turn the bandwidth limit to full, not 20M).
   * My oscilloscope is clocking the output of this at an eye-popping 110 MHz!
   */
  Logic::stop();                            // Stop the CCL so changes can be made

  /* Logic0 - CCL LUT0 */
  Logic0.enable = true;                     // Enable logic block 0
  Logic0.input0 = logic::in::feedback;             // feedback
  Logic0.input1 = logic::in::masked;               // masked
  Logic0.input2 = logic::in::masked;               // masked
  Logic0.output = logic::out::enable;              // Enable LUT0 OUT = PA3 (Tachi: D3 / Tsurugi: D19 / Kunai: D2)
  Logic0.filter = logic::filter::disable;          // No output filter enabled
  Logic0.truth = 0x01;                      // Set truth table: Invert, HIGH if input0 LOW
  Logic0.init();                            // Initialize logic block 0

  /* Event0 - EVSYS CHANNEL0 */
  Event0.stop();                            // Not Used. Stop Event0 (if it was running).

  /* Logic1 - CCL LUT1 */
  Logic1.enable = false;                    // Not using Logic1
  Logic1.init();                            // Initialize logic block 1 to apply the enable=false

  Logic::start();                           // Start the CCL hardware
}

void demo2() {
  /* Async demo 2: Adding a second Logic stage
   *
   * Run it through one of the other Logic blocks...
   * Remember that in::feedback gives the output of the even logic block, not the one it's used with
   * There's no pre-defined input for the output of the odd logic block (sadly).
   * Only feedback for the even one out of the pair, and link for the next highest.
   * This logic block will be set up to just echo it's input, rather than invert it.
   *
   * 'scope shows 55 MHz here (about exactly half)
   */
  Logic::stop();                            // Stop the CCL so changes can be made

  /* Logic0 - CCL LUT0 */
  Logic0.enable = true;                     // Enable logic block 0
  Logic0.input0 = logic::in::link;                 // Use output of next logic block (Logic1)
  Logic0.input1 = logic::in::masked;               // masked
  Logic0.input2 = logic::in::masked;               // masked
  Logic0.output = logic::out::enable;              // Enable LUT0 OUT = PA3 (Tachi: D3 / Tsurugi: D19 / Kunai: D2)
  Logic0.filter = logic::filter::disable;          // No output filter enabled
  Logic0.truth = 0x01;                      // Set truth table: Invert, HIGH if input0 LOW
  Logic0.init();                            // Initialize logic block 0

  /* Event0 - EVSYS CHANNEL0 */
  Event0.stop();                            // Not Used. Stop Event0 (if it was running).

  /* Logic1 - CCL LUT1 */
  Logic1.enable = true;                     // Enable logic block 1
  Logic1.input0 = logic::in::feedback;             // use output of even-numbered block, ie, block 0
  Logic1.input1 = logic::in::masked;               // masked
  Logic1.input2 = logic::in::masked;               // masked
  Logic1.output = logic::out::enable;              // enable logic block 1 output pin
  Logic1.filter = logic::filter::disable;          // No output filter enabled
  Logic1.truth = 0x02;                      // Set truth table: Copy, HIGH if input0 HIGH
  Logic1.init();                            // Initialize logic block 1

  Logic::start();                           // Start the CCL hardware

}
void demo3() {
  /* Async Demo 3: Using event channel stage
   *
   * Now, we will take an event channel and point it at the CCL0 generator, set CCL0 as the user of that event
   * This is a useful technique for getting a "feedback" for an odd logic block, as their "feedback" comes from
   * the even block they are associated with.
   *
   * Same as before - 55 MHz
   */
  Logic::stop();                            // Stop the CCL so changes can be made

  /* Logic0 - CCL LUT0 */
  Logic0.enable = true;                     // Enable logic block 0
  Logic0.input0 = logic::in::event_a;              // use event channel A - a virtual feedback
  Logic0.input1 = logic::in::masked;               // masked
  Logic0.input2 = logic::in::masked;               // masked
  Logic0.output = logic::out::enable;              // Enable LUT0 OUT = PA3 (Tachi: D3 / Tsurugi: D19 / Kunai: D2)
  Logic0.filter = logic::filter::disable;          // No output filter enabled
  Logic0.truth = 0x01;                      // Set truth table: Invert, HIGH if input0 LOW
  Logic0.init();                            // Initialize logic block 0

  /* Event0 - EVSYS CHANNEL0 */
  Event0.stop();                            // Stop Event0 (if it was running)
  Event0.set_generator(gen::ccl0_out);      // Use output of Logic0
  Event0.set_user(user::ccl0_event_a);      // Connect Event0 (carrying Logic0 output) to Logic0 event A input
  Event0.start();                           // Enable Event0

  /* Logic1 - CCL LUT1 */
  Logic1.enable = false;                    // Not using Logic1
  Logic1.init();                            // Initialize logic block 1 to apply the enable=false

  Logic::start();                           // Start the CCL hardware

}

void demo4() {
  /* Async demo 4: Using both second logic block and event stages
   *
   * Finally, we do both of those together
   * And this time I measure 37 MHz - that is to say, it would appear that each of these async stages
   * takes about the same length of time. Which probably shouldn't be particularly surprising.
   *
   * The actual frequency also varies significantly depending on the temperature and other particulars.
   *
   * Obviously, none of these are particularly useful, though you can do funny (if pointless) things like clock
   * the chip off of this (probably prescaled) as an external clock or something. A too-high-frequency signal
   * unmoored from any reference frequency is pretty useless...
   *
   * Next, we'll make some synchronized oscillators which will be more useful!
   *
   */
  Logic::stop();                            // Stop the CCL so changes can be made

  /* Logic0 - CCL LUT0 */
  Logic0.enable = true;                     // Enable logic block 0
  Logic0.input0 = logic::in::link;                 // link - use output of Logic1
  Logic0.input1 = logic::in::masked;               // masked
  Logic0.input2 = logic::in::masked;               // masked
  Logic0.output = logic::out::enable;              // Enable LUT0 OUT = PA3 (Tachi: D3 / Tsurugi: D19 / Kunai: D2)
  Logic0.filter = logic::filter::disable;          // No output filter enabled
  Logic0.truth = 0x01;                      // Set truth table: Invert, HIGH if input0 LOW
  Logic0.init();                            // Initialize logic block 0

  /* Event0 - EVSYS CHANNEL0 */
  Event0.stop();                            // Stop Event0 (if it was running)
  Event0.set_generator(gen::ccl0_out);      // Use output of Logic0 (virtual-feedback)
  Event0.set_user(user::ccl1_event_a);      // Connect Event0 (carrying Logic0 output) to Logic1 event A input
  Event0.start();                           // Enable Event0

  /* Logic1 - CCL LUT1 */
  Logic1.enable = true;                     // Enable logic block 1
  Logic1.input0 = logic::in::event_a;              // use event channel A
  Logic1.input1 = logic::in::masked;               // masked
  Logic1.input2 = logic::in::masked;               // masked
  Logic1.output = logic::out::enable;              // enable logic block 1 output pin
  Logic1.filter = logic::filter::disable;          // No output filter enabled
  Logic1.truth = 0x02;                      // Set truth table: Copy, HIGH if input0 HIGH
  Logic1.init();                            // Initialize logic block 0

  Logic::start();                           // Start the CCL hardware

}

void demo5() {
  /* Sync demo 1: The synchronizer delays each edge by 2 system clock cycles
   *
   * Same setup as original demo1 - except we enable the synchronizer which delays each transition by 2-3 clocks
   * In practice, we know that the signal will arrive early enough to always be delayed by only 2 clocks.
   * 2 clocks times 2 transitions gives us 4 clock cycles.
   *
   * We will use the default clock source (CLK_PER, which runs at F_CPU) here.
   *
   * Thus, we end up with a signal of F_CPU/4!
   *
   * This might be more useful!
   */
  Logic::stop();                            // Stop the CCL so changes can be made

  /* Logic0 - CCL LUT0 */
  Logic0.enable = true;                     // Enable logic block 0
  Logic0.input0 = logic::in::feedback;             // feedback
  Logic0.input1 = logic::in::masked;               // masked
  Logic0.input2 = logic::in::masked;               // masked
  Logic0.output = logic::out::enable;              // Enable LUT0 OUT = PA3 (Tachi: D3 / Tsurugi: D19 / Kunai: D2)
  Logic0.filter = logic::filter::sync;             // Synchronizer - 2 clock-cycle delay each transition
  Logic0.truth = 0x01;                      // Set truth table: Invert, HIGH if input0 LOW
  Logic0.init();                            // Initialize logic block 0

  /* Event0 - EVSYS CHANNEL0 */
  Event0.stop();                            // Not Used. Stop Event0 (if it was running).

  /* Logic1 - CCL LUT1 */
  Logic1.enable = false;                    // Not using Logic1
  Logic1.init();                            // Initialize logic block 1 to apply the enable=false

  Logic::start();                           // Start the CCL hardware
}

void demo6() {
  /* Sync demo 2: The Filter module adds 4 clock cycles per edge, 8 per cycle
   *
   * Same setup as previous - only filter instead of synchronizer. That takes 2 more clocks.
   * on each transition, total of 8. We aren't actually doing any filtering, just using the delay
   *
   * This way, we end up with a signal of F_CPU/8!
   *
   */
  Logic::stop();                            // Stop the CCL so changes can be made

  /* Logic0 - CCL LUT0 */
  Logic0.enable = true;                     // Enable logic block 0
  Logic0.input0 = logic::in::feedback;             // feedback
  Logic0.input1 = logic::in::masked;               // masked
  Logic0.input2 = logic::in::masked;               // masked
  Logic0.output = logic::out::enable;              // Enable LUT0 OUT = PA3 (Tachi: D3 / Tsurugi: D19 / Kunai: D2)
  Logic0.filter = logic::filter::filter;           // Filter - 4 clock-cycle delay each transition
  Logic0.truth = 0x01;                      // Set truth table: Invert, HIGH if input0 LOW
  Logic0.init();                            // Initialize logic block 0

  /* Event0 - EVSYS CHANNEL0 */
  Event0.stop();                            // Not Used. Stop Event0 (if it was running).

  /* Logic1 - CCL LUT1 */
  Logic1.enable = false;                    // Not using Logic1
  Logic1.init();                            // Initialize logic block 1 to apply the enable=false

  Logic::start();                           // Start the CCL hardware
}

void demo7() {
  /* Sync demo 3: Passing input through another logic block ADDS it's delay
   *
   * Same idea - only second logic block with a synchronizer user, and linked input.
   *
   * So delays are 8 + 4 = 12, and we end up with of F_CPU/12!
   * Can do the same with filter on both for F_CPU/16....
   * .
   */
  Logic::stop();                            // Stop the CCL so changes can be made

  /* Logic0 - CCL LUT0 */
  Logic0.enable = true;                     // Enable logic block 0
  Logic0.input0 = logic::in::link;                 // output from Logic1
  Logic0.input1 = logic::in::masked;               // masked
  Logic0.input2 = logic::in::masked;               // masked
  Logic0.output = logic::out::enable;              // Enable LUT0 OUT = PA3 (Tachi: D3 / Tsurugi: D19 / Kunai: D2)
  Logic0.filter = logic::filter::filter;           // Filter - 4 clock-cycle delay each transition
  Logic0.truth = 0x01;                      // Set truth table: Invert, HIGH if input0 LOW
  Logic0.init();                            // Initialize logic block 0

  /* Event0 - EVSYS CHANNEL0 */
  Event0.stop();                            // Not Used. Stop Event0 (if it was running).

  /* Logic1 - CCL LUT1 */
  Logic1.enable = true;                     // Enable logic block 1
  Logic1.input0 = logic::in::feedback;             // feedback (from logic0)
  Logic1.input1 = logic::in::masked;               // masked
  Logic1.input2 = logic::in::masked;               // masked
  Logic1.output = logic::out::enable;              // enable logic block 1 output pin
  Logic1.filter = logic::filter::sync;             // Synchronizer - 2 clock delay each transition
  Logic1.truth = 0x02;                      // Set truth table: Copy, HIGH if input0 HIGH
  Logic1.init();                            // Initialize logic block 1

  Logic::start();                           // Start the CCL hardware
}

void demo8() {
  /* Sync demo 4: Using other logic block as clock MULTIPLIES the delays
   *
   * Two logic blocks.
   * Logic0 is doing as it was in demo7. But in logic block 2, we connect it to input2
   * and set it to use input 2 as the clock. Then we use an event channel to get feedback for Logic1
   * as input0, and have this one also oscillate.
   *
   * With the filter on LUT1 clocked from the F_CPU/8 from LUT0, we we are generating an output of F_CPU/64
   *
   * What does this mean?
   * It means that with a few LUTs and into a prescaled clock on either a pin or an event channel at a wide
   * variety of fractions of the system clock speed. See the table:
   * Each frequency is listed only for the least-demanding combination of hardware that can do it:
   *
   * Divisor            Min logic blocks:             X = either an 8 (filter) or 4 (sync)
   * /4     /8          one
   * /12    /16         two                         - (8 + X)
   * /32    /64         two and one event channel   - (8 * X)
   * /20    /24         three,  no event channel    - (X + X  + X)
   * /48    /96   /128  three,  no event channel    - (8 + X) * X
   * /256   /512        three, one event channel    - (8 * 8  * X)
   * /28                four, no event channel      - (8 + 8 + 8 + 4)
   * /144   /192        four, no event channel      - (4 + 8) * (X + 8)
   * /384   /768  /1024 four, one event channel     - (X + 8) *  8 * X
   * /2048  /4096       four, two event channels    -  8 * 8  *  8 * X
   *
   * Next, we will demonstrate how to use this to clock a timer.
   */
  Logic::stop();                            // Stop the CCL so changes can be made

  /* Logic0 - CCL LUT0 */
  Logic0.enable = true;                     // Enable logic block 0
  Logic0.input0 = logic::in::feedback;             // feedback
  Logic0.input1 = logic::in::masked;               // masked
  Logic0.input2 = logic::in::link;                 // input2 (used as clock) from Logic1
  Logic0.clocksource = logic::clocksource::in2;    // Use input 2 as clock instead of the default
  Logic0.output = logic::out::enable;              // Enable LUT0 OUT = PA3 (Tachi: D3 / Tsurugi: D19 / Kunai: D2)
  Logic0.filter = logic::filter::filter;           // Filter - 4 clock delay each transition
  Logic0.truth = 0x01;                      // Set truth table: Invert, HIGH if input0 LOW
  Logic0.init();                            // Initialize logic block 0

  /* Event0 - EVSYS CHANNEL0 */
  Event0.stop();                            // Stop Event0 (if it was running)
  Event0.set_generator(gen::ccl1_out);      // Use output of Logic1
  Event0.set_user(user::ccl1_event_a);      // Connect Event0 (carrying Logic1 output) to Logic1 event A input
  Event0.start();                           // Enable Event0

  /* Logic1 - CCL LUT1 */
  Logic1.enable = true;                     // Enable logic block 1
  Logic1.input0 = logic::in::event_a;              // use event A, which is coming from output of Logic1 (ie, feedback for an odd block)
  Logic1.input1 = logic::in::masked;               // masked
  Logic1.input2 = logic::in::masked;               // masked
  Logic1.output = logic::out::enable;              // enable logic block 1 output pin
  Logic1.filter = logic::filter::filter;           // Filter - 4 clock delay each transition
  Logic1.truth = 0x01;                      // Set truth table: Invert, HIGH if input0 LOW
  Logic1.init();                            // Initialize logic block 1

  Logic::start();                           // Start the CCL hardware
}
void demo9a() {
  /* Using prescaled clocks 1: TCA0
   *
   * TCA0 is not a timer that really cries out for prescaling the clock going into it... it already HAS a prescaler that works fine and doesn't eat CCL
   * We'll do it for fun. Start with what we had last time - but let's change the
   * filter to a synchronizer on one of the blocks, just to make it different from normal PWM in some way (64 is default TCA0 prescaler, while 32 isn't
   * an option. And we'll add an event channel to bring carrying the output of Logic0. Then we'll reconfigure TCA0 to have it generate, uh. 8-bit PWM
   * on pin A2 at the twice frequency it normally would...
   * But it will be doing that without using it's own prescaler!
   * How exciting...
   *     ...
   *           ... Actually, this causes it's prescaler to not effect the TCA's clock rate.... but it does still effect that of a TCB using it!
   * So depending on how much you need which timer prescaled by, this might be just as good as getting a prescaler for a TCB (below).
   */

  Logic::stop();                            // Stop the CCL so changes can be made

  /* Logic0 - CCL LUT0 */
  Logic0.enable = true;                     // Enable logic block 0
  Logic0.input0 = logic::in::feedback;             // feedback
  Logic0.input1 = logic::in::masked;               // masked
  Logic0.input2 = logic::in::link;                 // input2 (used as clock) from Logic1
  Logic0.clocksource = logic::clocksource::in2;    // Use input 2 as clock instead of the default
  Logic0.output = logic::out::enable;              // Enable LUT0 OUT = PA3 (Tachi: D3 / Tsurugi: D19 / Kunai: D2)
  Logic0.filter = logic::filter::filter;           // Filter - 4 clock delay each transition
  Logic0.truth = 0x01;                      // Set truth table: Invert, HIGH if input0 LOW
  Logic0.init();                            // Initialize logic block 0

  /* Event0 - EVSYS CHANNEL0 */
  Event0.stop();                            // Stop Event0 (if it was running)
  Event0.set_generator(gen::ccl1_out);      // Use output of Logic1
  Event0.set_user(user::ccl1_event_a);      // Connect Event0 (carrying Logic1 output) to Logic1 event A input
  Event0.start();                           // Enable Event0

  /* Logic1 - CCL LUT1 */
  Logic1.enable = true;                     // Enable logic block 1
  Logic1.input0 = logic::in::event_a;              // use event A, which is coming from output of Logic1 (ie, feedback for an odd block)
  Logic1.input1 = logic::in::masked;               // masked
  Logic1.input2 = logic::in::masked;               // masked
  Logic1.output = logic::out::enable;              // enable logic block 1 output pin
  Logic1.filter = logic::filter::sync;             // Filter - 4 clock delay each transition
  Logic1.truth = 0x01;                      // Set truth table: Invert, HIGH if input0 LOW
  Logic1.init();                            // Initialize logic block 1

  /* Event1 - EVSYS CHANNEL1 */
  Event1.stop();                            // Stop Event1 (if it was running)
  Event1.set_generator(gen::ccl0_out);      // Use output of Logic0
  Event1.set_user(user::tca0_cnt_a);        // Connect Event1 (carrying Logic0 output) to TCA0 event A
  Event1.start();                           // Enable Event1

  Logic::start();                           // Start the CCL hardware

  /* TCA0 - Type A timer */
  TCA0.SPLIT.CTRLA &= ~TCA_SPLIT_ENABLE_bm; // disable and reset
  TCA0.SPLIT.CTRLESET = TCA_SPLIT_CMD_RESET_gc | TCA_SPLIT_CMDEN_BOTH_gc;

  // Set single-slope PWM mode, on CMP2
  TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP2EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc;

  // set PORTMUX so that the TCA waveform output appears on PORTA
  PORTMUX.TCAROUTEA = 0;

  // Count on positive edge, cnta event input
  TCA0.SINGLE.EVCTRL = TCA_SINGLE_EVACTA_CNT_POSEDGE_gc | TCA_SINGLE_CNTAEI_bm;

  // counting to 254 is the same thing that the timers are configured to do by default on WazamonoCore
  // so we're going to be expecting a 25% duty cycle
  TCA0.SINGLE.PER = 254;
  TCA0.SINGLE.CMP2 = 63;

  // And turn it back on!
  TCA0.SINGLE.CTRLA |= TCA_SINGLE_CLKSEL_DIV256_gc | TCA_SINGLE_ENABLE_bm;

}

void demo9b() {
  /* Divided Clocks: TCB0 - prescale it without dedicating the prescaler of a TCA to it!
   *
   * The type B timers are every AVR developers dream as utility timers. There are only two issues with them -
   * the first being that they're awful for PWM (but you have TCAs for that) and they don't have an independent
   * prescaler - they can either use a TCA prescaler, CLK_PER, or CLK_PER/2. Sometimes you really need to time
   * something that's a bit too long for CLK_PER/2, but you've got the TCA's way down at /64 or /256 for PWM
   * and you don't want to throw away all that accuracy. Or maybe you're writing a library, or code you expect
   * to be copy-pasted all over by people who don't understand it, who might be mixing it with other libraries?
   * (though, to be faaiiir, the official cores basically fall over like a house of cards the moment you turn off the prescal)
   *
   * The DU-series TCBs can clock on an event, so you can easily use the clock
   * generated as in the other examples to clock a TCB!
   *
   * We will use PWM frequency to demonstrate the change, though. Mostly because I have
   */

  Logic::stop();                            // Stop the CCL so changes can be made

  /* Logic0 - CCL LUT0 */
  Logic0.enable = true;                     // Enable logic block 0
  Logic0.input0 = logic::in::feedback;             // feedback
  Logic0.input1 = logic::in::masked;               // masked
  Logic0.input2 = logic::in::masked;               // masked
  Logic0.clocksource = logic::clocksource::clk_per;// Use CLK_PER (default) as clock source once more.
  Logic0.output = logic::out::enable;              // Enable LUT0 OUT = PA3 (Tachi: D3 / Tsurugi: D19 / Kunai: D2)
  Logic0.filter = logic::filter::filter;           // Filter - 4 clock-cycle delay each transition
  Logic0.truth = 0x01;                      // Set truth table: Invert, HIGH if input0 LOW
  Logic0.init();                            // Initialize logic block 0

  /* Event0 - EVSYS CHANNEL0 */
  Event0.set_generator(gen::ccl0_out);      // Use output of Logic0
  Event0.set_user(user::tcb0_cnt);          // Connect Event0 (carrying Logic0 output) to TCB0 count
  Event0.start();                           // Enable Event0

  /* Logic1 - CCL LUT1 */
  Logic1.enable = false;                    // Not using Logic1
  Logic1.init();                            // Initialize logic block 1 to apply the enable=false

  /* Event1 - EVSYS CHANNEL1 */
  Event1.stop();                            // Not Used. Stop Event1 (if it was running).

  Logic::start();                           // Start the CCL hardware

  /* TCB0 - Timer/Counter Type B */
  TCB0.CTRLA = 0;
  TCB0.CTRLA = TCB_CLKSEL_DIV2_gc | TCB_ENABLE_bm;  // Switch to clk_per clock & enable
  Serial.println("Before (CLK_PER/2): ~47 kHz");    // 24 / 2 = 12 MHz, 12 MHz / 255 count/cycle = ~47 kHz
  //we need to pick the PWM pin that the TCB is actually pointed at.
  //
  analogWrite(PIN_TCB0_WO_INIT, 128);               // Output some pwm to demo change in frequency
  delay(5000);                                      // That constant is the pin that we can assume the core configures that timer for by default/
  TCB0.CTRLA = 0;                                   // Disable TCB0
  TCB0.CTRLA = TCB_CLKSEL_EVENT_gc | TCB_ENABLE_bm; // Switch to event clock & enable
  Serial.println("After: ~11.7 kHz");               // 24 / 8 = 3 MHz, 3 MHz / 255 count/cycle = ~11.7 kHz

  /* To be clear: The PWM mode is just being used to give numbers, and something that looks good on a 'scope screen.
   * the typical use case for playing with the timer when you want a prescaler between the "8-bit analogWrite-like-PWM" prescalers and the
   * /4, /8, /12, and /16 range is timing that you need to be more precise than you can get with micros. Alternately, if you're taking over your
   * only TCA for 16-bit PWM, you might instead need to slow down the clock on the TCBs to get some plain 8-bit PWM down at a more tractable
   * frequency.
   *
   * One thing to remember is that you can have as many users as you want on a channel - you only need to make the desired clock rate once
   * and you can use it on everything.
   */

}



void loop() {
  demo1(); // Async - timer feeding back on itself reaching mindboggling speeds.
  Serial.println("Async: Single logic block, feedback - 100-110 MHz");
  delay(20000);
  Serial.println("Async: out -> second logic block -> link - 50-55 MHz");
  demo2();
  delay(20000);
  Serial.println("Async: out -> event -> input - 50-55 MHz");
  demo3();
  delay(20000);
  Serial.println("Async: out -> event -> second lut -> link - 33-37 MHz");
  /* So, it seems like it proceeds at about 5ns per "layer" of asynchronous black magic. (remember, 1 cycle is 2 passes)
   * Significant variation seen between parts, including speeds as low as 85 MHz and at least 120MHz */
  demo4();
  delay(20000);
  Serial.println("Clock dividing: Introducing synchronizer - 2 clocks/edge");
  demo5();
  delay(20000);
  Serial.println("Clock dividing: Filter - 4 clocks/edge");
  demo6();
  delay(20000);
  Serial.println("Clock dividing: One logic block into another");
  demo7();
  delay(20000);
  Serial.println("Clock dividing: One logic block clocked from another");
  demo8();
  delay(20000);
  Serial.println("Divided clocks: TCA0 - n");
  demo9a();
  delay(20000);
  TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm;
  Serial.println("Divided clocks: TCB gets an independent prescaler!");
  demo9b();
  delay(10000);
  digitalWrite(PIN_TCB0_WO_INIT, 0);
  pinMode(PIN_TCB0_WO_INIT, INPUT);

}
