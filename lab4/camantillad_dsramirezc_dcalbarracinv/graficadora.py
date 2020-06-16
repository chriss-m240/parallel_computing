import matplotlib.pyplot as plt
d=dict()
names=["720.jpg","1080.jpg","4k.jpg"]
for i in names:
    d[i]=dict()
    for j in range(3,16,2):
        d[i][j]=[0 for i in range(4)]
f=open("out.txt","r")
for k in names:
    for kernel in range(3,16,2):
        for j in range(4):
                for i in range(5):
                    s=f.readline().strip().split()
                    d[k][kernel][j]+=float(s[-1])
                d[k][kernel][j]/=5.0
ejex=[2**i for i in range(4)]
for i in range(3,16,2):
    plt.plot(ejex,d[names[0]][i],'-o',label=names[0])
    plt.plot(ejex,d[names[1]][i],'-o',label=names[1])
    plt.plot(ejex,d[names[2]][i],'-o',label=names[2])
    plt.legend()
    plt.title("kernel = "+str(i))
    plt.ylabel("time sg")
    plt.xlabel("#cpus")
    plt.savefig("graficas/AWSKernel"+str(i)+".jpg")
    plt.close()
for i in range(3,16,2):
    arr=[[0 for i in range(4)] for j in range(3)]
    for k in range(3):
        for j in range(4):
            arr[k][j]=(d[names[k]][i][0]/d[names[k]][i][j])
    plt.plot(ejex,arr[0],'-o',label=names[0])
    plt.plot(ejex,arr[1],'-o',label=names[1])
    plt.plot(ejex,arr[2],'-o',label=names[2])
    plt.legend()
    plt.title("speedup cpus = "+str(i))
    plt.ylabel("speedup")
    plt.xlabel("#cpus")
    plt.savefig("graficas/speedupAWSkernel"+str(i)+".jpg")
    plt.close()
