/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thbeil <thbeil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/10 11:16:43 by thbeil            #+#    #+#             */
/*   Updated: 2026/10/19 00:00:00 by thbeil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <limits.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <pthread.h>
# include <sys/time.h>
# include <time.h>
# include <unistd.h>

typedef enum e_scheduler
{
	FIFO,
	EDF
}	t_scheduler;

typedef struct s_config
{
	unsigned int		number_of_coders;
	long long unsigned	time_to_bo;
	long long unsigned	time_to_compile;
	long long unsigned	time_to_debug;
	long long unsigned	time_to_refactor;
	unsigned int		number_of_compiles_required;
	long long unsigned	dongle_cd;
	t_scheduler			scheduler;
}	t_config;

typedef struct s_heap_entry
{
	int					coder_num;
	long long unsigned	key;
}	t_heap_entry;

typedef struct s_heap
{
	t_heap_entry	*data;
	int				size;
	int				capacity;
}	t_heap;

typedef struct s_dongle
{
	int					is_taken;
	long long unsigned	released_at;
	pthread_mutex_t		mutex;
	pthread_cond_t		cond;
	int					never_used;
	t_heap				wait_heap;
}	t_dongle;

typedef struct s_coder
{
	int					number;
	unsigned int		how_much_compile;
	long long unsigned	when_last_compile;
	long long unsigned	ready_since;
	t_dongle			*left_dongle;
	t_dongle			*right_dongle;
	pthread_t			self;
	pthread_mutex_t		mutex;
}	t_coder;

typedef struct s_simulation
{
	t_config			config;
	t_coder				*coders;
	t_dongle			*dongles;
	long long unsigned	t0;
	pthread_mutex_t		log_mutex;
	int					stop;
	pthread_mutex_t		stop_mutex;
	unsigned int		dongles_ready;
	unsigned int		coders_ready;
}	t_simulation;

typedef struct s_thread_arg
{
	t_simulation	*sim;
	t_coder			*coder;
}	t_thread_arg;

/* parsing_utils.c */
int					number_of_parameters(int nb);
int					is_valid_positive_number(char *str, int index);
int					is_fifo_or_edf(char *str);
int					check_value(t_config *config);

/* parsing.c */
void				set_value(t_config *config, char *str, int index);

/* utils.c */
long long			ft_atoll(char *str);
long long unsigned	get_current_time(t_simulation *sim);

/* init.c */
int					init_simulation(t_simulation *sim, t_config *config);
int					init_dongles(t_simulation *sim);
int					link_dongles(t_simulation *sim);

/* dongle.c */
void				take_dongle(t_simulation *sim, t_coder *me,
						t_dongle *dongle);

/* dongle_utils.c */
void				take_both_dongles(t_simulation *sim, t_coder *me);
void				release_dongle(t_simulation *sim, t_dongle *dongle);

/* sync.c */
int					should_stop(t_simulation *sim);
void				set_stop(t_simulation *sim);
void				sleep_ms(t_simulation *sim, long long unsigned ms);
void				log_state(t_simulation *sim, t_coder *coder, char *msg);

/* coder.c */
int					do_compile(t_simulation *sim, t_coder *coder);
void				do_debug_and_refactor(t_simulation *sim, t_coder *coder);
void				*coder_routine(void *arg);

/* monitor.c */
int					check_burnout(t_simulation *sim);
int					all_coders_done(t_simulation *sim);
void				*monitor_routine(void *arg);

/* cleanup.c */
void				cleanup(t_simulation *sim);

/* main.c */
int					parse_args(int ac, char **av, t_config *config);
int					launch_threads(t_simulation *sim, t_thread_arg *args,
						pthread_t *monitor);
void				join_threads(t_simulation *sim, pthread_t monitor);

/* heap.c */
int					heap_init(t_heap *h, int capacity);
void				heap_sift_up(t_heap *h, int i);
void				heap_sift_down(t_heap *h, int i);

/* heap_ops.c */
void				heap_free(t_heap *h);
void				heap_push(t_heap *h, t_heap_entry e);
void				heap_pop(t_heap *h);
void				heap_remove(t_heap *h, int coder_num);

#endif
