from sqlalchemy import *

metadata = MetaData()

user_table = Table(
	"user",
	metadata,
	Column('id', Integer, primary_key=True),
	Column('name', String(30)),
	Column('full_name', String),
	Column('has_access', Boolean),
)

card_table = Table(
	"card",
	metadata,
	Column('id', Integer, primary_key=True),
	Column('user_id', ForeignKey('user.id'), nullable=False, unique=False),
	Column('card_id', String, nullable=False),
)

engine = create_engine(
	"sqlite+pysqlite:////tmp/test2.db",
	connect_args={"check_same_thread": False},
	echo=True,
	future=True,
)
metadata.drop_all(engine)
metadata.create_all(engine)
with engine.connect() as conn:
	print("CREANDO")
	conn.execute( insert(user_table), [
		{"id": 1, "name": "andres", "full_name": "Andres Botero", "has_access": True},
		{"id": 2, "name": "german", "full_name": "German Ro", "has_access": False},
	])
	conn.execute( insert(card_table), [
		{"id": 1, "user_id": 1, "card_id": "9048485E"},
		{"id": 2, "user_id": 1, "card_id": "9C7A653B"},
		{"id": 3, "user_id": 2, "card_id": "78426B58"},
		{"id": 4, "user_id": 2, "card_id": "A7220004"},
	])
	conn.commit()

def get_user(conn, in_name):
	return conn.execute(
		select(user_table).where(user_table.c.name == in_name)
	)

def get_card(conn, in_card_id):
	return conn.execute(
		select(card_table, user_table).join(user_table).where(card_table.c.card_id == in_card_id)
	)
