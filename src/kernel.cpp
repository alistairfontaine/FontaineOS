/*
   We wrap our main entrypoint in extern "C".
   This stops the C++ compiler from mangling the function name,
   allowing our 'boot.s' assembly file to find and call 'kernel_main' perfectly.
*/
extern "C" void kernel_main() {
    /*
       0xB8000 is the hardcoded x86 physical memory address for the VGA text frame buffer.
       We mark it 'volatile' so the C++ compiler doesn't optimize away our direct memory writes.
    */
    volatile char* video_memory = (volatile char*)0xB8000;

    /* Our initial test string message */
    const char* message = "Welcome to FontaineOS, Alistair!";

    int i = 0;
    while (message[i] != '\0') {
        /* Write the actual text character byte into the layout array channel */
        video_memory[i * 2] = message[i];

        /*
           Write the visual style attribute byte right next to the character byte.
           0x0E represents a brilliant Yellow text color on a classic Black background matrix.
        */
        video_memory[i * 2 + 1] = 0x0E;

        i++;
    }

    /*
       Keep the kernel alive in an infinite loop so it doesn't drop out
       of execution execution branches.
    */
    while (true) {
        // CPU remains idle and running safely here
    }
}
