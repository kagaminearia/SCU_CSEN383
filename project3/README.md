# Multi-threaded Ticket Sellers

This project simulates a multi-threaded ticket selling system for a concert.

## Compilation

To compile the project, first create a `bin` directory, then run the `gcc` command:

```bash
mkdir -p bin
gcc -o bin/ticket_seller src/main.c src/seller_threads.c src/seating.c src/clock.c src/logging.c src/metrics.c -Iinclude -lpthread
```

This will create an executable file at `bin/ticket_seller`.

## Running the Simulation

To run the simulation, execute the compiled program with the number of customers per seller as a command-line argument.

For example, to run the simulation with 5 customers per seller and save the output to a file:

```bash
./bin/ticket_seller 5 > output_N5.txt
```

The simulation will run for 60 simulated minutes, or until all tickets are sold. The output will be saved to the specified file.
