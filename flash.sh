#!/bin/sh

openocd -f interface/stlink.cfg -f target/stm32h7x_dual_bank.cfg \
  -c "program build/m7core.elf verify; program build/m4core.elf reset verify exit"