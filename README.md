# GestionDeConge
Gestion de congé réalisé en c++ et Qt

L'application est composé de 4 niveaux d'utilisateurs :

- Utilisateur simple
- Responsable
- Rh
- Responsable Rh

Chaque niveau a différentes options de gestion :

- L'utilisateur simple peut créer ou consulter ses demandes de congé.
- L'administrateur peut créer une demande de congé, valider les demandes de son service et les transmettre au service rh et en supprimer
- Le Rh peut créer une demande de congé, valider les demandes (le solde de congé est vérifié automatiquement), supprimer des demandes, créer des utilisateurs et leurs attribuer un solde de congé.
- Le responsable Rh n'a besoin que de valider ou supprimer les demandes de congé des Rh.
