/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nakhalil <nakhalil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 16:03:58 by nakhalil          #+#    #+#             */
/*   Updated: 2025/07/11 15:48:43 by nakhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*str;
	size_t	len1;
	size_t	len2;
	const char *p1;
	const char *p2;

	if (s1)
		p1 = s1;
	else
		p1 = "";
	if (s2)
		p2 = s2;
	else
		p2 = "";
	len1 = ft_strlen(p1);
	len2 = ft_strlen(p2);
	str = malloc(len1 + len2 + 1);
	if (!str)
		return (NULL);
	ft_memcpy(str, p1, len1);
	ft_memcpy(str + len1, p2, len2 + 1);
	return (str);
}
