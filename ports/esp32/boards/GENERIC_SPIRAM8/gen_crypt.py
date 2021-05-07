import secrets

with open('modules/cr.py', 'w') as f:
    f.write('''
from ucryptolib import aes as _

def c(k={}):
    return _(k, 2, {})
'''.format(secrets.token_bytes(32), secrets.token_bytes(16)))

