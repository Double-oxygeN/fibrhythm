import os, random, sequtils, strutils

iterator nanika(a: Positive, b: Positive, density: float = 0.5): bool =
  var
    s: seq[bool] = repeat(false, a)
    t: seq[bool] = repeat(false, b)
    u: seq[bool]

  while s == t[(b-a)..^1]:
    for i in 0..<a:
      s[i] = (rand(1.0) < density)
    for i in 0..<b:
      t[i] = (rand(1.0) < density)

  u = t & s & t
  s &= t
  t = u

  while true:
    for x in s:
      yield x

    u = s & t
    s = t
    t = u

when isMainModule:
  var
    c: int = 0
    f: Positive = 1
    s: Positive = 1
    n: int = 1024

  when declared(commandLineParams):
    for i, argv in commandLineParams():
      if i == 0: n = argv.parseInt()
      elif i == 1: f = argv.parseInt()
      elif i == 2: s = argv.parseInt()

  for b in nanika(f, s):
    stdout.write (if b: 1 else: 0)
    inc(c)
    if c >= n:
      echo ""
      break
