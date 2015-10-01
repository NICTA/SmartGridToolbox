import csv
import datetime

day = 60 * 24
week = day * 7

reader = csv.reader(open('loads_orig.txt'), delimiter=';')
next(reader)
dat = map(lambda x : (datetime.datetime.strptime(' '.join([x[0], x[1]]),
                                                 '%d/%m/%Y %H:%M:%S'),
                      x[2], x[3]),
          reader)

dat = filter(lambda x : x[0].date().month in [12, 1, 2], dat)

while True:
    cur = next(dat)
    if (cur[0].time() == datetime.time(23,59)):
        # dat will start at 00:00:00
        break

dat = list(dat)
chunks = [dat[x:x+week] for x in range(0, len(dat), week)]
chunks = [
    chunk for chunk in chunks 
    if not any(row[1] == '?' or row[2] == '?' for row in chunk)
    and int((chunk[-1][0] - chunk[0][0]).total_seconds() / 60.0) + 1 == week
]

i = 0
for chunk in chunks:
    i = i + 1
    fname = 'loads/load_{:d}.txt'.format(i)

    with open(fname, 'w+') as f:
        out_mins = 0
        for row in chunk:
            f.write('{:d} {:f}+{:f}j\n'.format(
                out_mins, 
                float(row[1]) * 0.001, float(row[2]) * 0.001))
            out_mins += 1
