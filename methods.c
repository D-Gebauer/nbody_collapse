#include "methods.h"

struct vec {
    double x;
    double y;
    double z;
};

struct particle {
    struct vec coordinates;
    struct vec velocities;
    double mass;
};

void progressbar(int current, int total) {
    float percent = 100 * (float)current / (float)total;
    int filled = 50 * current / total;
    //char bar[] = filled * '█' + '-' * (50 - filled);

    printf("\r%i/%i |", current, total);

    for (int i = 0; i < filled; i++)
    {
        printf("\r");
        printf("█");
    }
    for (int i = 0; i < (50 - filled); i++)
    {
        printf("_");
    }

    printf("| %f%% Complete\r", percent);

    if (current==total) {
        printf("\n");
    }
}

void progress_simple(int current, int total) {
    printf("\r%i/%i\r", current, total);
}

struct particle *setup(int number, double temperature) {
    gsl_rng_env_setup();
    const gsl_rng_type* T = gsl_rng_default;
    gsl_rng* r = gsl_rng_alloc(T);

    double sigma = sqrt(k_B * temperature / particle_mass);

    struct particle *ptr = malloc(number * sizeof(struct particle));

    for (int i = 0; i < number; ++i) {
        ptr[i].mass = particle_mass;
        ptr[i].coordinates.x = gsl_rng_uniform(r)*1e24;
        ptr[i].coordinates.y = gsl_rng_uniform(r)*1e24;
        ptr[i].coordinates.z = gsl_rng_uniform(r)*1e24;
        ptr[i].velocities.x = gsl_ran_gaussian_ziggurat(r, sigma);
        ptr[i].velocities.y = gsl_ran_gaussian_ziggurat(r, sigma);
        ptr[i].velocities.z = gsl_ran_gaussian_ziggurat(r, sigma);
    }

    gsl_rng_free(r);
    return ptr;
}
