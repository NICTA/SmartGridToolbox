import sgt

def main():
    print(sgt.Phase.BAL)
    print(sgt.Phase.A)
    print(sgt.Phases(sgt.Phase.N))
    print(sgt.Phases(sgt.Phase.A | sgt.Phase.B | sgt.Phase.N))
    netw = sgt.Network(1)
    print(netw)

if __name__ == "__main__":
    main()
