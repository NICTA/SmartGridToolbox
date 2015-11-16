import sgt

def main():
    print(sgt.Phase.BAL)
    print(sgt.Phase.A)
    print(sgt.Phases(sgt.Phase.N))
    print(phase_or(sgt.Phase.A | sgt.Phase.B))
    netw = sgt.Network(1)
    print(netw)
    test()

if __name__ == "__main__":
    main()
