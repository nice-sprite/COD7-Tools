import re
path = r"C:\Users\coxtr\source\repos\COD7 Scripting\COD7 Scripting\commands.txt"
out_path = r'C:\Users\coxtr\source\repos\COD7 Scripting\COD7 Scripting\commands_test.json'

fin = open(path)
fout = open(out_path, 'w')

lines = fin.readlines()
print('''
{

    "command_count": 3214
    "commands": [
        
''')

fout.write('''
{

    "command_count": 3214
    
''')
for line in lines:
    name = line.split(' ')[0].strip()
    description = line.split('//')
    if len(description) > 1:
        description = description[1].strip().replace('"', r"'")
    else:
        description = 'No Decscription.'
    pattern = re.compile(r'".*"')
    def_val = pattern.search(line)
    if def_val:
        def_val = def_val.group()
    else:
        def_val = '"No default value."'

    #print('{"command_name": ' + f'"{name}", ' + '"default_value": ' +  f'{def_val}, ' + '"description": ' + f'"{description}"' + '},')
    fout.write(
    f'"{name}":\n'
    '{\n'
    f'\t"command_name": "{name}",\n'
    f'\t"default_value": {def_val},\n'
    f'\t"description": "{description}"\n'
    '},\n')
    
fin.close()
fout.close()
