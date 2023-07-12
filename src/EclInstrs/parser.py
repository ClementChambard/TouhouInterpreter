class GrammarRule:
    def __init__(self, tokens: list[str], size: int, out: str, aststr):
        self.tokens = tokens
        self.size = size
        self.out = out
        self.aststr = aststr
        self.ast = eval(aststr)
        self.i = 0


logLevel = 0


def readDotGram(filename):
    text = ""
    with open(filename, "r") as f:
        text = f.read()
    return eval(text)


def parse(rules, stateTable, lexfunc, text):
    tokens = [[t.parseStr(), t] for t in lexfunc(text)]

    stack = [0]
    symbolStack = []

    while True:
        action = stateTable[stack[-1]].get(
            tokens[0][0], stateTable[stack[-1]]["DEFAULT"]
        )

        if action[0] == "Err":
            print("Error: unexpected", tokens[0][0], "in state", stack)
            [print(s) for s in stateTable]
            return

        if action[0] == "S":
            if logLevel > 0:
                print("Shift", tokens[0][0])
            symbolStack.append(tokens[0][1])
            tokens = tokens[1:]
            stack.append(action[1])

        if action[0] == "R":
            reduceRule = rules[action[1]]
            if logLevel > 0:
                print("Reduce", reduceRule)
            toReduce = []
            if reduceRule.size > 0:
                toReduce = symbolStack[-reduceRule.size :]
                symbolStack = symbolStack[: -reduceRule.size]
                stack = stack[: -reduceRule.size]
            symbolStack.append(reduceRule.ast(toReduce))
            action = stateTable[stack[-1]].get(
                reduceRule.out, stateTable[stack[-1]]["DEFAULT"]
            )
            if action[0] != "S":
                print("ERROR")
                return
            stack.append(action[1])

        if action[0] == "OK":
            if logLevel > 0:
                print("OK")
            return symbolStack[0]


import lexer

rules, stateTable = readDotGram("touhouInstr.gramm")
import sys

if len(sys.argv) < 2:
    exit(1)

filecontent = ""
with open(sys.argv[1], "r") as f:
    filecontent = f.read()
ast = parse(rules, stateTable, lexer.lexAll, filecontent)

for ins in ast:
    print(f"_ins({ins[1]}, {ins[0]})", end=" ")
    if len(ins[2]) != 0:
        for arg in ins[2]:
            print(f"{arg[0]}({arg[1]}) ", end="")
        print("_args", end=" ")
    print(ins[3])
