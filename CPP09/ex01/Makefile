NAME        = RPN

C++         = c++

FLAGS		= -Wall -Wextra -Werror -std=c++98

SRCS        = main.cpp \
              RPN.cpp

OBJS        = ${SRCS:.cpp=.o}

REMOVE		= rm -f ${OBJS}

all:        ${NAME}

${NAME}:    ${OBJS}
			${C++} ${FLAGS} ${OBJS} -o ${NAME}

%.o:		%.cpp
			${C++} ${FLAGS} -c $< -o $@

clean:
			${REMOVE}

fclean:     clean
			rm -f ${NAME}

re:         fclean all

.PHONY:     all clean fclean re