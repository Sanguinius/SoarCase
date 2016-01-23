typedef struct
{
    double q; //process noise covariance
    double r; //measurement noise covariance
    double x; //value
    double p; //estimation error covariance
    double k; //kalman gain
} Kalman_state;

Kalman_state


Kalman_init ( double q, double r, double p, double intial_value )
{
    Kalman_state result;
    result.q = q;
    result.r = r;
    result.p = p;
    result.x = intial_value;

    return result;
}

void Kalman_update ( Kalman_state* state, double measurement )
{
    //prediction update
    //omit x = x
    state->p = state->p + state->q;

    //measurement update
    state->k = state->p / ( state->p + state->r );
    state->x = state->x + state->k * ( measurement - state->x );
    state->p = ( 1 - state->k ) * state->p;
}

