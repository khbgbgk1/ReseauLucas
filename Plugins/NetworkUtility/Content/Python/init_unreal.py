import unreal
import NetworkLagAndSeamless

def initialize() :
    NetworkLagAndSeamless.main()
    unreal.log("NetworkLagAndSeamless enregistré avec succès.")


if __name__ == "__main__" :
    unreal.log("Démarrage du Plugin de NetworkLagAndSeamless...")
    initialize()
    unreal.log("Le Plugin de NetworkLagAndSeamless est chargé.")
    
