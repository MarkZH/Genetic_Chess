#ifndef TESTING_H
#define TESTING_H

//! \file

//! \brief Run through a battery of tests to verify the operation of most of the program.
//!
//! \returns True if all tests passed
bool run_tests();

//! \brief Run a speed test for Gene Board scoring, Move submission, and Board copying.
void run_speed_tests();

//! \brief Run a timed test of legal move generation to check that chess rules are implemented correctly.
//!
//! \returns True if all tests pass.
bool run_perft_tests();

#endif // TESTING_H
