#include <algorithm> // std::reverse
#include <cmath>
#include <limits>
#include <vector>
#include "../include/ts.h"
#include "../include/dtw.h"

namespace TSdist {

static const double NOT_VISITED = -1;
static const int STEP_DIAG = 0;
static const int STEP_LEFT = 1;
static const int STEP_UP = 2;
//static const double DBL_MAX = std::numeric_limits<double>::max();

// ================================================================================================
/* Lp Norm */
// ================================================================================================
double lnorm(const TimeSeriesBase& x, const TimeSeriesBase& y,
             int p, int time_x, int time_y)
{
    double result = 0;

    for (int k = 0; k < x.numVars(); k++) {
        result += std::abs(std::pow(x[time_x][k] - y[time_y][k], p));
    }

    return std::pow(result, 1.0 / p);
}

// ================================================================================================
/* Which direction to take when traversing CM */
// ================================================================================================
int which_direction(double (&tpl)[3], double diag, double left, double up,
                    double diag_weight, double local_cost)
{
    tpl[STEP_DIAG] = (diag == NOT_VISITED) ? DBL_MAX : diag + diag_weight * local_cost;
    tpl[STEP_LEFT] = (left == NOT_VISITED) ? DBL_MAX : left + local_cost;
    tpl[STEP_UP] = (up == NOT_VISITED) ? DBL_MAX : up + local_cost;

    // which direction has the least associated cost?
    int direction = (tpl[STEP_LEFT] < tpl[STEP_DIAG]) ? STEP_LEFT : STEP_DIAG;
    direction = (tpl[STEP_UP] < tpl[direction]) ? STEP_UP : direction;

    return direction;
}

// ================================================================================================
/* DTW distance */
// ================================================================================================
double computeDTW(const TimeSeriesBase& x, const TimeSeriesBase& y,
                  int window_size, int p, int diag_weight)
{
    if (x.numVars() != y.numVars())
        throw("Series must have the same number of variables.");

    if (p < 1)
        throw("Parameter p must be positive.");

    if (diag_weight != 1 && diag_weight != 2)
        throw("Diagonal weight can only be 1 or 2.");

    int nx = x.length();
    int ny = y.length();

    // local/global cost matrix, only 2 rows to implement memory-saving version
    double CM[2][ny + 1];
    // possible directions to take when traversing CM
    double tuple_direction[3];

    int i, j, direction;
    double local_cost;

    // initialization of first row and first column
    for (j = 0; j <= ny; j++) CM[0][j] = NOT_VISITED;
    CM[1][0] = NOT_VISITED;

    // first value, must set here to avoid multiplying by step
    CM[1][1] = std::pow(lnorm(x, y, p, 0, 0), p);

    // dynamic programming
    for (i = 1; i <= nx; i++)
    {
        int j1, j2;

        // adjust limits depending on window
        if (window_size < 1) {
            j1 = 1;
            j2 = ny;

        } else {
            j1 = std::ceil((double) i * ny / nx - window_size);
            j2 = std::floor((double) i * ny / nx + window_size);

            j1 = j1 > 1 ? j1 : 1;
            j2 = j2 < ny ? j2 : ny;
        }

        for (j = 1; j <= ny; j++)
        {
            // very first value already set above
            if (i == 1 && j == 1) continue;

            if (j < j1 || j > j2) {
                // cell outside of window
                CM[i % 2][j] = NOT_VISITED;
                continue;
            }

            // l-norm for single time-point, p only affects multivariate series
            local_cost = std::pow(lnorm(x, y, p, i-1, j-1), p);

            // which direction has the least associated cost?
            direction = which_direction(tuple_direction,
                                        CM[(i - 1) % 2][j - 1],
                                        CM[i % 2][j - 1],
                                        CM[(i - 1) % 2][j],
                                        diag_weight,
                                        local_cost);

            CM[i % 2][j] = tuple_direction[direction];
        }
    }

    // calculate p-root on the very last value
    return std::pow(CM[nx % 2][ny], 1.0 / p);
}

// ================================================================================================
/* Normalized DTW distance */
// ================================================================================================
double computeNormalizedDTW(const TimeSeriesBase& x, const TimeSeriesBase& y,
                            int window_size, int p)
{
    return computeDTW(x, y, window_size, p, 2) / (x.length() + y.length());
}

// ================================================================================================
/* DTW distance with backtracking */
// ================================================================================================
double backtrackDTW(const TimeSeriesBase& x, const TimeSeriesBase& y,
                    int window_size, int p, int diag_weight,
                    std::vector<int>& idx, std::vector<int>& idy)
{
    if (x.numVars() != y.numVars())
        throw("Series must have the same number of variables.");

    if (p < 1)
        throw("Parameter p must be positive.");

    if (diag_weight != 1 && diag_weight != 2)
        throw("Diagonal weight can only be 1 or 2.");

    // make sure indices are empty initially
    idx.clear();
    idy.clear();

    int nx = x.length();
    int ny = y.length();

    // local/global cost matrix
    double CM[nx + 1][ny + 1];
    // possible directions to take when traversing CM
    double tuple_direction[3];

    int i, j, direction;
    double local_cost;

    // initialization of first row and first column
    for (i = 0; i <= nx; i++) CM[i][0] = NOT_VISITED;
    for (j = 1; j <= ny; j++) CM[0][j] = NOT_VISITED;

    // first value, must set here to avoid multiplying by step
    CM[1][1] = std::pow(lnorm(x, y, p, 0, 0), p);

    // dynamic programming
    for (i = 1; i <= nx; i++)
    {
        int j1, j2;

        // adjust limits depending on window
        if (window_size < 1) {
            j1 = 1;
            j2 = ny;

        } else {
            j1 = std::ceil((double) i * ny / nx - window_size);
            j2 = std::floor((double) i * ny / nx + window_size);

            j1 = j1 > 1 ? j1 : 1;
            j2 = j2 < ny ? j2 : ny;
        }

        for (j = 1; j <= ny; j++)
        {
            // very first value already set above
            if (i == 1 && j == 1) continue;

            if (j < j1 || j > j2) {
                // cell outside of window
                CM[i][j] = NOT_VISITED;
                continue;
            }

            // l-norm for single time-point, p only affects multivariate series
            local_cost = std::pow(lnorm(x, y, p, i-1, j-1), p);

            // which direction has the least associated cost?
            direction = which_direction(tuple_direction,
                                        CM[i - 1][j - 1],
                                        CM[i][j - 1],
                                        CM[i - 1][j],
                                        diag_weight,
                                        local_cost);

            /*
             * I can use the same matrix to save both cost values and steps taken by shifting
             * the indices left and up for direction. Since the loop advances row-wise, the
             * appropriate values for the cost will be available, and the unnecessary ones are
             * replaced by steps along the way.
             */

            CM[i][j] = tuple_direction[direction];
            CM[i - 1][j - 1] = (double) direction;
        }
    }

    // backtracking loop, always start at end of series
    i = nx - 1;
    j = ny - 1;
    idx.push_back(i);
    idy.push_back(j);

    while(!(i == 0 && j == 0)) {
        // re-use as temporary value
        local_cost = CM[i][j];

        if (local_cost == (double) STEP_DIAG) {
            i--;
            j--;

        } else if (local_cost == (double) STEP_LEFT) {
            j--;

        } else if (local_cost == (double) STEP_UP) {
            i--;

        } else {
            throw("Invalid direction matrix computed.");
        }

        idx.push_back(i);
        idy.push_back(j);
    }

    // adjust order
    std::reverse(idx.begin(), idx.end());
    std::reverse(idy.begin(), idy.end());

    // calculate p-root on the very last value
    return std::pow(CM[nx][ny], 1.0 / p);
}

// ================================================================================================
/* Normalized DTW distance with backtracking */
// ================================================================================================
double backtrackNormalizedDTW(const TimeSeriesBase& x, const TimeSeriesBase& y,
                              int window_size, int p,
                              std::vector<int>& idx, std::vector<int>& idy)
{
    return backtrackDTW(x, y, window_size, p, 2, idx, idy) / (x.length() + y.length());
}

}
