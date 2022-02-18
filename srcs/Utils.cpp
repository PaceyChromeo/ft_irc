#include "../includes/Utils.hpp"
#include <iostream>
#include <cstring>

void	free_tab(char **tab)
{
	int	i;

	i = 0;
	while (tab[i])
	{
		free(tab[i]);
		tab[i] = NULL;
		i++;
	}
	free(tab);
}

size_t	ft_strlen(const char *str)
{
	size_t	i;

	i = 0;
	while (str[i] != '\0')
		i++;
	return (i);
}

void	*ft_calloc(size_t nmemb, size_t size)
{
	char	*tab;
	size_t	i;
	size_t	max;

	max = nmemb * size;
	tab = (char *)malloc(max);
	if (!tab)
		return (NULL);
	i = 0;
	while (max--)
	{
		tab[i] = 0;
		i++;
	}
	return ((void *)tab);
}

static char	*ft_strndup(char const *s, size_t n)
{
	char	*dest;
	size_t	i;

	i = 0;
	dest = (char *)malloc(n + 1);
	if (dest == NULL)
		return (NULL);
	while (i < n)
	{
		dest[i] = s[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}

static int	ft_countwords(char *str, char set)
{
	int	i;
	int	len;

	len = 0;
	i = 0;
	if (ft_strlen(str) == 0)
		return (0);
	if (str[0] != set)
		len++;
	while (str[i])
	{
		if (str[i] == set)
		{
			if (str[i] == set && (str[i + 1] != set && str[i + 1] != '\0'))
				len++;
		}
		i++;
	}
	return (len);
}

char	**ft_split(char const *str, char set)
{
	char	**tab;
	int		i;
	int		m_tab;
	int		len_word;

	m_tab = 0;
	i = -1;
	if (!str)
		return (NULL);
	tab = (char **)ft_calloc(sizeof(tab), (ft_countwords((char *)str, set) + 1));
	if (!tab)
		return (NULL);
	while (str[++i])
	{
		len_word = 0;
		if (str[i] != set)
		{
			while (str[i + len_word] != set && str[i + len_word] != '\0')
				len_word++;
			tab[m_tab++] = ft_strndup(str + i, len_word);
			i = i + len_word - 1;
		}
	}
	tab[m_tab] = 0;
	return (tab);
}

char	*ft_strdup(const char *src)
{
	size_t	i;
	size_t	len;
	char	*dest;

	len = ft_strlen((char *)src);
	dest = (char*)malloc(sizeof(char) * len + 1);
	if (dest == NULL)
		return (0);
	i = 0;
	while (i <= len)
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	size_t	i;
	size_t	b;
	char	*newstr;

	i = -1;
	b = start;
	if (!s)
		return (NULL);
	if (start > ft_strlen(s))
		return (ft_strdup(""));
	while (s[b] && ++i < len)
		b++;
	newstr = (char *)ft_calloc(sizeof(char), i + 1);
	if (!newstr)
		return (NULL);
	i = 0;
	while (s[start] && i < len)
	{
		newstr[i] = s[start];
		i++;
		start++;
	}
	newstr[i] = '\0';
	return (newstr);
}

int	check_recv(char *msg){
	if (!strncmp(msg, "CAP LS", 5)){
		return (1);
	}
	else if (!strncmp(msg, "MODE ", 5)){
		return (2);
	}
	else if (!strncmp(msg, "JOIN ", 5)){
		return (3);
	}
	else if (!strncmp(msg, "PING ", 5)){
		return (4);
	}
	return (0);
}