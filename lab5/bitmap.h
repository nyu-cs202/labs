#pragma once

#include <stdbool.h>
#include <stdint.h>

int	alloc_diskblock(void);
bool	diskblock_is_free(uint32_t blockno);
void	free_diskblock(uint32_t blockno);
