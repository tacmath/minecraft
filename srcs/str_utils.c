#include "scop.h" 

char *getOption(char *option, int ac, char **av, char *object) {
    int n;

    n = 0;
    while (++n < ac) {
        if (!strcmp(option, av[n])) {
            if (n + 1 < ac && strcmp(av[n + 1], object))
                return (av[n + 1]);
            else
                return (0);
        }
    }
    return (0);
}

char *getObjectFile(int ac, char **av) {
    int n;
    char *ret;

    n = 0;
    while (++n < ac) {
        ret = strrchr(av[n], '.');
        if (ret && !strcmp(ret, ".obj"))
            return (av[n]);
    }
    return (0);
}

char *ft_strjoin(char const *s1, char const *s2)
{
	int		len = strlen(s1);
	char	*str;

	if (!(s1 && s2))
		return (0);
	if (!(str = malloc(sizeof(char) * (len + strlen(s2) + 1))))
		return (0);
    memcpy(str, s1, len);
    strcpy(str + len, s2);
	return (str);
}

char *ft_replaceStr(char *str, char *ref, char *replace) {
    char *new;
    char *tmp;
    int n;

    if (!(tmp = strstr(str, ref)) ||
        !(new = malloc(strlen(str) - strlen(ref) + strlen(replace) + 1)))
        return (0);
    n = tmp - str;
    memcpy(new, str, n);
    strcpy(new + n, replace);
    strcpy(new + n + strlen(replace), tmp + strlen(ref));
    return (new);
}