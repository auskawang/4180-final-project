#include "state.h"

SystemState current_state = STATE_WAITING_USER_INPUT;

void change_to_running() {
  current_state = STATE_RUNNING;
}

SystemState get_state() {
  return current_state;
}