def convert_base(num, to_base=10, from_base=10):
    if isinstance(num, str):
        n = int(num, from_base)
    else:
        n = int(num)
    alphabet = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    if n < to_base:
        return alphabet[n]
    else:
        return convert_base(n // to_base, to_base) + alphabet[n % to_base]
s = list(input().split())
answ = ''
hex_ = ['0', '1', '2', '3', '4', '5', '6', '7', '8',
       '9', 'A', 'B', 'C', 'D', 'E', 'F']
for i in range(len(s)):
    err = 0
    if s[i].isdigit():
        answ += '0x' + convert_base(s[i], 16, 10) + ' '
    elif s[i][0] == '0' and s[i][1] == 'x' and len(s[i]) > 2:
        for j in range(2, len(s[i])):
            if s[i][j] not in hex_:
                answ += s[i] + ' '
                err = 1
                break
        if err == 0:
            answ += convert_base(s[i][2::], 10, 16) + ' '
    else:
        answ += s[i] + ' '
print(answ[:-1:])
