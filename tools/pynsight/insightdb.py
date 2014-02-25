import insight.debugger 

f = insight.debugger.file
r = insight.debugger.run
ms = insight.debugger.microstep
s = insight.debugger.step
c = insight.debugger.cont
b = insight.debugger.breakpoint
watch = insight.debugger.watchpoint
d = insight.debugger.delete_breakpoints
P = insight.debugger.prog
pc = insight.debugger.pc
ep = insight.debugger.entrypoint
cond = insight.debugger.cond

from insight.debugger import *
if len (sys.argv) == 2:
    file (sys.argv[1])
elif len (sys.argv) == 3:
    file (sys.argv[1],sys.argv[2])

print "Insight Debugger"
print "Try help(insight.debugger) to get information on debugger commands."
print
