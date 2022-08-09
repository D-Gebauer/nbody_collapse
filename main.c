//includes
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "methods.h"
// constants
double G = 6.6743e-11;
double pi = 3.141592653;
double k_B = 1.380649e-23;
double particle_mass = 1;

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

struct particle *setup(int number, double temperature, double max_r) {
    gsl_rng_env_setup();
    const gsl_rng_type* T = gsl_rng_default;
    gsl_rng* r = gsl_rng_alloc(T);

    double sigma = sqrt(k_B * temperature / particle_mass);

    struct particle *ptr = malloc(number * sizeof(struct particle));

    for (int i = 0; i < number; ++i) {
        ptr[i].mass = particle_mass;
        ptr[i].coordinates.x = gsl_rng_uniform(r)*max_r;
        ptr[i].coordinates.y = gsl_rng_uniform(r)*max_r;
        ptr[i].coordinates.z = gsl_rng_uniform(r)*max_r;
        ptr[i].velocities.x = gsl_ran_gaussian_ziggurat(r, sigma);
        ptr[i].velocities.y = gsl_ran_gaussian_ziggurat(r, sigma);
        ptr[i].velocities.z = gsl_ran_gaussian_ziggurat(r, sigma);
    }

    gsl_rng_free(r);
    return ptr;
}

struct vec grav_force(struct vec *a, struct vec *b, double softening) {
    struct vec r =  {a->x - b->x, a->y-b->y, a->z - b->z};
    double r_abs = sqrt(r.x*r.x+r.y*r.y+r.z*r.z);
    struct vec F = {0,0,0};

    if (r_abs) {
        double force = -G * particle_mass * particle_mass / (r_abs * r_abs + softening);
        F.x = force * r.x / (r_abs);
        F.y = force * r.y / (r_abs);
        F.z = force * r.z / (r_abs);
        return F;
    } else {
        printf("Error: r_abs = 0");
        return F;
    }
}

void update_v(struct particle *particles, int n, double delta_t, double softening) {
    struct vec F;
    for (int i = 0; i < n; ++i) {
        for (int j = i; j < n; ++j) {
            if (i!=j) {
                F = grav_force(&particles[i].coordinates, &particles[j].coordinates, softening);
                particles[i].velocities.x += F.x * delta_t / particle_mass;
                particles[i].velocities.y += F.y * delta_t / particle_mass;
                particles[i].velocities.z += F.z * delta_t / particle_mass;
                particles[j].velocities.x -= F.x * delta_t / particle_mass;
                particles[j].velocities.y -= F.y * delta_t / particle_mass;
                particles[j].velocities.z -= F.z * delta_t / particle_mass;
            }
        }
    }
}

void update_coords(struct particle *particles, int n, double delta_t) {
    for (int i = 0; i < n; ++i) {
        particles[i].coordinates.x += particles[i].velocities.x * delta_t;
        particles[i].coordinates.y += particles[i].velocities.y * delta_t;
        particles[i].coordinates.z += particles[i].velocities.z * delta_t;
    }
}

void simulate_timestep(struct particle *particles, int n, double delta_t, double softening) {
    update_v(particles, n, delta_t, softening);
    update_coords(particles, n, delta_t);
    //write to file
}

void save_step(struct particle *particles) {
    //write to file
    //for testing:
    printf("%lf, %lf, %lf\n", particles[0].coordinates.x, particles[0].coordinates.y, particles[0].coordinates.z);
}

int main() {
    int n = 5000;
    double T = 1e24;
    double max = 100;
    struct particle *test;
    double softening = 1e-4;
    double delta_t = 1e-3;
    int time_steps = 1000;

    struct particle *particles = setup(n,T,100);
    for (int i = 0; i < time_steps; ++i) {
        //progressbar(i, time_steps);
        simulate_timestep(particles, n, delta_t, softening);
        if (!(i%20)) {save_step(particles);}
    }


    return 0;
}
