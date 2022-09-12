 FROM python:3.8
 ENV PYTHONUNBUFFERED 1
 RUN mkdir /code
 WORKDIR /code
 RUN pip install pipenv
 ADD requirements.txt /code/
 ADD Pipfile /code/
 ADD Pipfile.lock /code/
 RUN pipenv install
 RUN pipenv run pip freeze > requirements.txt
 RUN pip install -r requirements.txt
 ADD . /code/
