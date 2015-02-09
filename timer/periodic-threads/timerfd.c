/* 
 * This is a demonstration of periodic threads using the Linux timerfd
 * interface which was introduced in GNU libc 2.8 and kernel 2.6.25.
 */

/* ------------------------------------------------------------------------- */
/*                                                                           */
/* Copyright (C) 2009 Chris Simmonds (chris@2net.co.uk)                      */
/*                                                                           */
/* This program is free software; you can redistribute it and/or modify      */
/* it under the terms of the GNU General Public License as published by      */
/* the Free Software Foundation; either version 2 of the License, or         */
/* (at your option) any later version.                                       */
/*                                                                           */
/* This program is distributed in the hope that it will be useful,           */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU          */
/* General Public License for more details.                                  */
/*                                                                           */
/* You should have received a copy of the GNU General Public License         */
/* along with this program; if not, write to the Free Software               */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA   */
/*                                                                           */
/* ------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/timerfd.h>

struct periodic_info
{
	int timer_fd;
	unsigned long long wakeups_missed;
};

static int make_periodic (unsigned int period, struct periodic_info *info)
{
	int ret;
	unsigned int ns;
	unsigned int sec;
	int fd;
	struct itimerspec itval;

	/* Create the timer */
	fd = timerfd_create (CLOCK_MONOTONIC, 0);
	info->wakeups_missed = 0;
	info->timer_fd = fd;
	if (fd == -1)
		return fd;

	/* Make the timer periodic */
	sec = period/1000000;
	ns = (period - (sec * 1000000)) * 1000;
	itval.it_interval.tv_sec = sec;
	itval.it_interval.tv_nsec = ns;
	itval.it_value.tv_sec = sec;
	itval.it_value.tv_nsec = ns;
	ret = timerfd_settime (fd, 0, &itval, NULL);
	return ret;
}

static void wait_period (struct periodic_info *info)
{
	unsigned long long missed;
	int ret;

	/* Wait for the next timer event. If we have missed any the
	   number is written to "missed" */
	ret = read (info->timer_fd, &missed, sizeof (missed));
	if (ret == -1)
	{
		perror ("read timer");
		return;
	}

	info->wakeups_missed += missed;
}

static int thread_1_count;
static int thread_2_count;

static void *thread_1 (void *arg)
{
	struct periodic_info info;

	printf ("Thread 1 period 10ms\n");
	make_periodic (10000, &info);
	while (1)
	{
		thread_1_count++;
		wait_period (&info);
	}
	return NULL;
}

static void *thread_2 (void *arg)
{
	struct periodic_info info;

	printf ("Thread 2 period 20ms\n");
	make_periodic (20000, &info);
	while (1)
	{
		thread_2_count++;
		wait_period (&info);
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	pthread_t t_1;
	pthread_t t_2;

	printf ("Periodic threads using timerfd\n");

	pthread_create (&t_1, NULL, thread_1, NULL) ;
	pthread_create (&t_2, NULL, thread_2, NULL) ;
	
	sleep (10);
	printf ("thread1 %d iterations\n", thread_1_count);
	printf ("thread2 %d iterations\n", thread_2_count);
	return 0 ;
}

