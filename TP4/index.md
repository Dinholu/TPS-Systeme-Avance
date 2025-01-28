# TP 4 : DOCKER


## Proposer un service 

1. Récupération de l'image Docker 

```bash
docker pull jenkins/jenkins:lts-jdk17
```

![pull jenkins](ressources/image-0.png)

![Liste conteneur](ressources/image-1.png)


2. Démarrage du conteneur 

```bash
docker run -it -p 8080:8080 -p 5000:5000 jenkins/jenkins:lts-jdk17
```

![run jenkins](ressources/image-3.png)

Après avoir mis le mdp et installer les ressources

![run terminal](ressources/image-4.png)


3. La vérification de la disponibilité du service

```bash 
docker ps -a
``` 
![docker ps](ressources/image-5.png)

4. L'arrêt du conteneur 

```bash 
docker stop jenkins-container
``` 

![docker stop](ressources/image-6.png)



## Service from Scratch

1. Récupération des données 

```bash 
docker pull tomcat:jdk17
```

![alt text](ressources/image-7.png)

2. Création du DockerFile 

![alt text](ressources/image.png)

3. Création de l'image et deploiement

```bash 
docker build -t jenkins:1.0 .
```


4. Test du services

```bash 
docker run -it -p 8080:8080 jenkins:1.1
``` 

Et après téléchager les outils et mis le mdp spécifié 
![alt text](ressources/image-8.png)


