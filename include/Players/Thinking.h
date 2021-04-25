#ifndef THINKING_H
#define THINKING_H

//! \file

//! \brief Indicates the expected format of thinking output for AIs interfacing with an external GUI.
enum class Thinking_Output_Type : size_t
{
    NO_THINKING,
    CECP,
    UCI
};

#endif // THINKING_H
