TIMESTAMP=$(date +"%F_%T")
FILENAME="backup_$TIMESTAMP.sql"
pg_dump -U <postgres> -h <127.0.0.1> -p <5432> <crypto_bot> > $FILENAME
python3 upload_to_dropbox.py $FILENAME
echo "Резервная копия успешно загружена в Dropbox."
rm $FILENAME
