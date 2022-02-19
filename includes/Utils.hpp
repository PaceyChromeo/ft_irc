#ifndef UTILS_HPP
#define UTILS_HPP

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

char    **ft_split(char const *s, char c);
char	*ft_substr(char const *s, unsigned int start, size_t len);
char	*ft_strdup(const char *src);
void	free_tab(char **tab);
#endif
