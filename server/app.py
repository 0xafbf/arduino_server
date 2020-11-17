
from flask import Flask
import db

app = Flask(__name__)

@app.route("/")
def root():
#    import pdb; pdb.set_trace()
    with db.engine.connect() as conn:
        print("got connection")
        print(conn)
        user = next(db.get_user(conn, "andres"))
        conn.close()
    
    return user.full_name

@app.route("/<name>")
def get_user(name):
#    import pdb; pdb.set_trace()
    with db.engine.connect() as conn:
        print("got connection")
        print(conn)
        user = next(db.get_user(conn, name))
        conn.close()
    
    return user.full_name

@app.route("/card/<card_id>")
def get_card_info(card_id):
    card_has_access = False
    with db.engine.connect() as conn:
        print("got connection")
        print(conn)
        for card in db.get_card(conn, card_id):
            card_has_access = card.has_access
        conn.close()
    
    return str(card_has_access)
    


if __name__ == '__main__':
    
    app.run(debug=True, host="0.0.0.0")
    
