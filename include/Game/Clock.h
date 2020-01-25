#ifndef Clock_H
#define Clock_H

#include <array>
#include <chrono>

#include "Color.h"

class Board;
class Game_Result;
class CECP_Mediator;
class UCI_Mediator;

//! This class represents the dual-clock game timers used in tournament chess.
class Clock
{
    public:
        //! Constructs a game clock for timing games.
        //
        //! \param duration_seconds The initial amount of time on the clock.
        //!        If this is zero, then the clock is inactive and will not stop the game.
        //! \param moves_to_reset The number of moves before the clocks are reset to the initial time.
        //! \param increment_seconds Amount of time to add to a player's clock after every move.
        //! \param starting_turn Which player's clock to start upon calling start().
        //! \param previous_start_time If the clock for a game is being replaced by another clock (for example, a GUI
        //!        changes time control midgame), then this parameter can be used to preserve the actual start of the
        //!        current game.
        Clock(double duration_seconds = 0.0,
              size_t moves_to_reset = 0,
              double increment_seconds = 0.0,
              Color starting_turn = WHITE,
              std::chrono::system_clock::time_point previous_start_time = {}) noexcept;

        //! Stop the current player's clock and restart the opponent's clock.
        Game_Result punch(const Board& board) noexcept;

        //! Undo the last clock punch (time is not added).
        void unpunch() noexcept;

        //! Stop both clocks.
        void stop() noexcept;

        //! Start the moving player's clock at the start of a game.
        //
        //! This method also records the start time of the game for use in Board::print_game_record().
        void start() noexcept;

        //! Returns the amount of time left for the given player.
        //
        //! \param color The color of the player whose time is being queried.
        double time_left(Color color) const noexcept;

        //! The number of moves left before the clocks reset to the initial time.
        //
        //! \param color The color of the player being queried.
        size_t moves_until_reset(Color color) const noexcept;

        //! The player for whom the clock is running.
        Color running_for() const noexcept;

        //! The amount of time left on the clock that is currently running.
        double running_time_left() const noexcept;

        //! Are clocks currently running?
        bool is_running() const noexcept;

        //! Returns the date and time when start() was called.
        std::chrono::system_clock::time_point game_start_date_and_time() const noexcept;

        //! The intitial time on the clocks at the start of the game (and added after moves_to_reset()).
        double initial_time() const noexcept;

        //! How much time is added to a player's clock after every move.
        double increment(Color color) const noexcept;

        //! How many moves must be played before the clocks are reset to their initial times.
        size_t moves_per_time_period() const noexcept;

        //! Was the clock used for a game?
        bool is_in_use() const noexcept;

    private:
        using fractional_seconds = std::chrono::duration<double>;

        std::array<fractional_seconds, 2> timers;
        std::array<size_t, 2> moves_to_reset_clocks{0, 0};

        fractional_seconds initial_start_time;
        std::array<fractional_seconds, 2> increment_time;
        size_t move_count_reset;
        int initial_time_set_count = 0;

        Color whose_turn;
        bool clocks_running = false;

        std::chrono::system_clock::time_point game_start_date_time;
        std::chrono::steady_clock::time_point time_previous_punch;

        friend class CECP_Mediator;
        friend class UCI_Mediator;

        //! Adjust the time on the clocks.
        //
        //! \param player Indicates which clock to adjust.
        //! \param new_time_seconds The new amount of time left on that clock.
        //!
        //! This method is used by the Players derived from Outside_Communicator to adjust
        //! the clock according to instructions from chess GUIs using the CECP protocol.
        void set_time(Color player, double new_time_seconds) noexcept;
        void set_increment(Color player, double new_increment_time_seconds) noexcept;
        void set_next_time_reset(size_t moves_to_reset) noexcept;
};

#endif // Clock_H
