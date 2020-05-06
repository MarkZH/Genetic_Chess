#ifndef Clock_H
#define Clock_H

#include <array>
#include <chrono>
using namespace std::chrono_literals;

#include "Color.h"

class Board;
class Game_Result;
class CECP_Mediator;
class UCI_Mediator;

//! \file

//! \brief Specifies whether time is added or the clock reset when the moves per time period have been made.
enum class Time_Reset_Method
{
    ADDITION,
    SET_TO_ORIGINAL
};

//! \brief This class represents the dual-clock game timers used in tournament chess.
class Clock
{
    public:
        //! \brief The unit of time used by this clock system is seconds represented by a double floating-point type.
        using seconds = std::chrono::duration<double>;

        //! \brief Constructs a game clock for timing games.
        //!
        //! \param duration_seconds The initial amount of time on the clock.
        //!        If this is zero, then the clock is inactive and will not stop the game.
        //! \param moves_to_reset The number of moves before the clocks are reset to the initial time.
        //! \param increment_seconds Amount of time to add to a player's clock after every move.
        //! \param reset_method Whether time is added or the clock is reset after the specified number of moves.
        //! \param starting_turn Which player's clock to start upon calling start().
        //! \param previous_start_time If the clock for a game is being replaced by another clock (for example, a GUI
        //!        changes time control midgame), then this parameter can be used to preserve the actual start of the
        //!        current game.
        Clock(seconds duration_seconds = 0.0s,
              size_t moves_to_reset = 0,
              seconds increment_seconds = 0.0s,
              Time_Reset_Method reset_method = Time_Reset_Method::ADDITION,
              Piece_Color starting_turn = Piece_Color::WHITE,
              std::chrono::system_clock::time_point previous_start_time = {}) noexcept;

        //! \brief Stop the current player's clock and restart the opponent's clock.
        Game_Result punch(const Board& board) noexcept;

        //! \brief Undo the last clock punch (time is not added).
        void unpunch() noexcept;

        //! \brief Stop both clocks.
        void stop() noexcept;

        //! \brief Start the moving player's clock at the start of a game.
        //!
        //! This method also records the start time of the game for use in Board::print_game_record().
        void start() noexcept;

        //! \brief Returns the amount of time left for the given player.
        //!
        //! \param color The color of the player whose time is being queried.
        seconds time_left(Piece_Color color) const noexcept;

        //! \brief The number of moves left before the clocks reset to the initial time.
        //!
        //! \param color The color of the player being queried.
        size_t moves_until_reset(Piece_Color color) const noexcept;

        //! \brief The player for whom the clock is running.
        Piece_Color running_for() const noexcept;

        //! \brief The amount of time left on the clock that is currently running.
        seconds running_time_left() const noexcept;

        //! \brief Are clocks currently running?
        bool is_running() const noexcept;

        //! \brief Returns the date and time when start() was called.
        std::chrono::system_clock::time_point game_start_date_and_time() const noexcept;

        //! \brief The intitial time on the clocks at the start of the game (and added after moves_to_reset()).
        seconds initial_time() const noexcept;

        //! \brief How much time is added to a player's clock after every move.
        seconds increment(Piece_Color color) const noexcept;

        //! \brief How many moves must be played before the clocks are reset to their initial times.
        size_t moves_per_time_period() const noexcept;

        //! \brief How time is added once a timing period (moves to reset): adding or reseting to the original time.
        Time_Reset_Method reset_mode() const noexcept;

        //! \brief Was the clock used for a game?
        bool is_in_use() const noexcept;

    private:
        std::array<seconds, 2> timers;
        std::array<size_t, 2> moves_since_clock_reset{0, 0};

        seconds initial_start_time;
        std::array<seconds, 2> increment_time;
        size_t move_count_reset;
        Time_Reset_Method method_of_reset;

        Piece_Color whose_turn;
        bool clocks_running = false;

        std::chrono::system_clock::time_point game_start_date_time;
        std::chrono::steady_clock::time_point time_previous_punch;

        friend class CECP_Mediator;
        friend class UCI_Mediator;

        //! \brief Adjust the time and other aspects on the clocks.
        //!
        //! \param player Indicates which clock to adjust.
        //! \param new_time_seconds The new amount of time left on that clock.
        //!
        //! These methods are used by the classes derived from Outside_Communicator to adjust
        //! the clock according to instructions from chess GUIs.
        void set_time(Piece_Color player, seconds new_time_seconds) noexcept;
        void set_increment(Piece_Color player, seconds new_increment_time_seconds) noexcept;
        void set_next_time_reset(size_t moves_to_reset) noexcept;
        void set_reset_method(Time_Reset_Method new_method) noexcept;
};

#endif // Clock_H
