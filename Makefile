NAME = codexion
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread

INCS_DIR = coders/includes
SRCS_DIR = coders/srcs
OBJS_DIR = objs

SRCS = $(SRCS_DIR)/main.c $(SRCS_DIR)/utils.c $(SRCS_DIR)/parser.c $(SRCS_DIR)/init.c \
	   $(SRCS_DIR)/pqueue.c $(SRCS_DIR)/coders.c $(SRCS_DIR)/monitor.c $(SRCS_DIR)/log.c
OBJS = $(patsubst $(SRCS_DIR)/%.c,$(OBJS_DIR)/%.o,$(SRCS))
INCS = -I$(INCS_DIR)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
