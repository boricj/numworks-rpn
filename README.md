# numworks-rpn

This is a RPN calculator for NumWorks.

## Getting started

```
git clone https://github.com/boricj/numworks-rpn.git apps/rpn

# Quick test run on simulator
make PLATFORM=simulator EPSILON_APPS='rpn settings'

# Complete firmware on hardware
make EPSILON_APPS='rpn graph code statistics probability solver calculation sequence regression settings' epsilon_flash
```

## Key mapping
- EXE: `DUP`
- Ans: `OVER`
- Right parenthesis: `SWAP`
- Left parenthesis: `ROT`
- Space (alpha+'-'): Opposite
- Equal (shift+pi): Switch between exact and approximation modes
