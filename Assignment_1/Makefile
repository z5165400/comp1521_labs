BOARD?=board1.s
ITERS?=3

run: life.s
	@spim -file $<

test: life life.s
	@echo $(ITERS) | ./life > c.out
	@echo $(ITERS) | spim -file life.s > mips.out
	@diff c.out mips.out

life.s: $(BOARD) prog.s
	cat $< prog.s > $@

life: life.c
	clang --std=c99 $< -o $@

clean:
	rm life.s life c.out mips.out
