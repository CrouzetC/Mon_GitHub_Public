

from pylab import *
from math import *
from numpy import *


N=256
f=0.1
Te=1

t = [t for t in range(N)]
sinus = [sin(2*pi*i*f) for i in t]

plot(t,sinus)
show()

F = []
for k in range(N) :
    fk = k/N
    F += [0]
    for i in range(N) :
        F[k] += sinus[i]*2.71**(-1.0j*2.0*pi*fk*i*Te)
    # Pour avoir directement les coeffs : F[k] = F[k]/N

F_f = 0
for k in range(N) :
    F_f += sinus[k]*2.71**(-1.0j*2.0*pi*f*k*Te)
# F_f = F_f/N

print("X(f) = "+str(abs(F_f)))

F = [abs(e) for e in F ]
plot(t,F)
show()

plot(t,[abs(e) for e in fft.fft(sinus)])
show()
