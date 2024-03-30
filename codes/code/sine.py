
import math

f = open("trianglewave.txt", "a")

for i in range(360):
    #sine
    #num = math.sin(math.radians(i))

    #sawtooth
    #num = (i - 180) / 180

    #triangle
    # num = (i - 90) / 90
    # if i > 180:
    #     num = (-i + 270) / 90

    #square
    # num = -1
    # if i > 180:
    #     num = 1
    

    f.write("{:.5f}".format(num) + ",\n")


f.close()