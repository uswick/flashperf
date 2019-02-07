/* Copyright (C) 
 * 2019 - Udayanga Wickramasinghe
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */
/** 
 * @file fperf.h
 * @brief Flash perf counters -- Quick and easy way to collect PMU stats using Linux perf_events
 * @author Udayanga Wickramasinghe
 * @version 1.0
 * @date 2019-02-07
 */
#ifndef __H_FLASH_PERF_H__
#define __H_FLASH_PERF_H__
#include <asm/unistd.h>
#include <linux/perf_event.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

typedef enum{
  E_INS_COUNT = 0,
  E_L1_D_MISSES,
  E_LL_D_MISSES,
  E_TLB_D_MISSES,
  E_NODE_L_ACCESSES,
  E_NODE_L_MISSES,
  NO_EVENT
} perf_evt_t;

typedef struct perf_cvalue {
  perf_evt_t e;
  uint32_t type;
  uint64_t config;
} perf_cvalue_t;

#define EVENT(name)  E_ ## name

#define PERF_DECL()\
  static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,\
			    int cpu, int group_fd, unsigned long flags) {\
    int ret;\
    ret = syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);\
    return ret;\
  }\
\
static perf_cvalue_t __perf_cvalues[] = {\
  {.e=E_INS_COUNT, .type=PERF_TYPE_HARDWARE, .config=PERF_COUNT_HW_INSTRUCTIONS},\
  {.e=E_L1_D_MISSES, .type=PERF_TYPE_HW_CACHE, .config=((PERF_COUNT_HW_CACHE_L1D) | (PERF_COUNT_HW_CACHE_OP_READ << 8) |\
                      (PERF_COUNT_HW_CACHE_RESULT_MISS << 16))},\
  {.e=E_LL_D_MISSES, .type=PERF_TYPE_HW_CACHE, .config=((PERF_COUNT_HW_CACHE_LL) | (PERF_COUNT_HW_CACHE_OP_READ << 8) |\
                      (PERF_COUNT_HW_CACHE_RESULT_MISS << 16))},\
  {.e=E_TLB_D_MISSES, .type=PERF_TYPE_HW_CACHE, .config=((PERF_COUNT_HW_CACHE_DTLB) | (PERF_COUNT_HW_CACHE_OP_READ << 8) |\
                      (PERF_COUNT_HW_CACHE_RESULT_MISS << 16))},\
  {.e=E_NODE_L_ACCESSES, .type=PERF_TYPE_HW_CACHE, .config=((PERF_COUNT_HW_CACHE_NODE) | (PERF_COUNT_HW_CACHE_OP_READ << 8) |\
                      (PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16))},\
  {.e=E_NODE_L_MISSES, .type=PERF_TYPE_HW_CACHE, .config=((PERF_COUNT_HW_CACHE_NODE) | (PERF_COUNT_HW_CACHE_OP_READ << 8) |\
                      (PERF_COUNT_HW_CACHE_RESULT_MISS << 16))},\
};\
\
  struct perf_event_attr __perf_pe;\
  long long		 __perf_cnt;\
  int			 __perf_fd;

#define PERF_START(val)\
  do{\
  memset(&__perf_pe, 0, sizeof(struct perf_event_attr));\
  __perf_pe.type	   = (__perf_cvalues[EVENT(val)].type);\
  __perf_pe.size	   = sizeof(struct perf_event_attr);\
  __perf_pe.config	 = (__perf_cvalues[EVENT(val)].config);\
  __perf_pe.disabled       = 1;\
  __perf_pe.exclude_kernel = 1;\
  __perf_pe.exclude_hv     = 1;\
  __perf_fd = perf_event_open(&__perf_pe, 0, -1, -1, 0);\
  if (__perf_fd == -1) {\
    fprintf(stderr, "Error opening leader %llx\n", __perf_pe.config);\
    exit(EXIT_FAILURE);\
  }\
\
  ioctl(__perf_fd, PERF_EVENT_IOC_RESET, 0);\
  ioctl(__perf_fd, PERF_EVENT_IOC_ENABLE, 0);\
\
  }while(0);

#define PERF_END(val)\
  do{\
  ioctl(__perf_fd, PERF_EVENT_IOC_DISABLE, 0);\
  read(__perf_fd, &__perf_cnt, sizeof(long long));\
  printf("perf event=%s %lld \n",#val,  __perf_cnt);\
  close(__perf_fd);\
  }while(0);

#endif
