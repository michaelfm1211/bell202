# bell202

This project is a very crude [Bell 202](https://en.wikipedia.org/wiki/Bell_202_modem)
software modulator. It currently only supports 8N1 mode (8 data bits, no parity,
1 start bit, 1 stop bit) and prepends messages with 16 synchronization bits. The
modulated audio is outputted to standard output in uncompressed (PCM) WAVE
format.
