#ifndef DDPG_HPP_SENTRY
#define DDPG_HPP_SENTRY

#define PURSUADERS_AMOUNT 3

struct ddpg_action {
        double action[PURSUADERS_AMOUNT * 2];
};

struct ddpg_response {
        double state[PURSUADERS_AMOUNT * 2 + 2];
        double reward;
        bool done;
};

int ddpg_connect();

void get_action(int ddpg_conn, ddpg_action *action_buf);

void response_action(int ddpg_conn, ddpg_response *response_buf);

#endif
