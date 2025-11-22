enum HydrationState {
    HYDRATED,
    NEEDS_WATER,
    CRITICAL
};

void update_hydration_status(float grams);
HydrationState get_hydration_state();

