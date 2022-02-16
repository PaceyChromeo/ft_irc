#include "Utils.hpp"


static unsigned    int        ft_count_words(char *s, char c)
{
    unsigned int        count;

    count = 0;
    while (*s)
    {
        if (*s == '\0')
            break ;
        else if (*s != c)
        {
            count++;
            while (*s && (*s != c))
                s++;
        }
        else
            s++;
    }
    return (count);
}

static char                *ft_strndup(char const *s, size_t n)
{
    char    *cp;
    size_t    i;

    i = 0;
    cp = (char *)malloc(n + 1);
    if (cp == NULL)
        return (NULL);
    while (i < n)
    {
        cp[i] = s[i];
        i++;
    }
    cp[i] = '\0';
    return (cp);
}

char                    **ft_split(char const *s, char c)
{
    unsigned int    i;
    char            **tab_words;
    unsigned int    j;
    unsigned int    k;

    if (!s || !(tab_words = (char **)malloc((sizeof(char*))
        * (ft_count_words((char *)s, c) + 1))))
        return (NULL);
    i = 0;
    k = 0;
    while (s[i])
    {
        while (s[i] == c)
            i++;
        j = i;
        while (s[i] && s[i] != c)
            i++;
        if (i > j)
        {
            tab_words[k] = ft_strndup(s + j, i - j);
            k++;
        }
    }
    tab_words[k] = NULL;
    return (tab_words);
}