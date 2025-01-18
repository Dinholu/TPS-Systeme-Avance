# TP 4 : DOCKER


## Proposer un service 

1. Récupération de l'image Docker 

```bash
docker pull jenkins/jenkins:lts-jdk17
```

![pull jenkins](image.png)

![Liste conteneur](image-1.png)


2. Démarrage du conteneur 

```bash
docker run -it -p 8080:8080 -p 5000:5000 jenkins/jenkins:lts-jdk17
```

![alt text](image-3.png)

Après avoir mis le mdp et installer les ressources

![alt text](image-4.png)


3. La vérification de la disponibilité du service

```bash 
docker ps -a
``` 
![alt text](image-5.png)

4. L'arrêt du conteneur 

```bash 
docker stop jenkins-container
``` 

![alt text](image-6.png)



## Service from Scratch

