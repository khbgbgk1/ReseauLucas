import unreal
import importlib

# Valeur modifiable manuellement dans ce fichier
# Sauvegardez le fichier et cliquez sur le bouton pour actualiser dans UE
LAG_VALUE = 500 

def get_context():
    """Récupère un contexte valide pour exécuter des commandes console."""
    # En priorité le monde du PIE (Play In Editor)
    if unreal.EditorLevelLibrary.get_game_world():
        return unreal.EditorLevelLibrary.get_game_world()
    # Sinon le monde de l'éditeur
    return unreal.EditorLevelLibrary.get_editor_world()

def set_network_lag():
    """Charge la valeur et l'applique avec un contexte valide."""
    import NetworkLagAndSeamless
    importlib.reload(NetworkLagAndSeamless)
    
    val = NetworkLagAndSeamless.LAG_VALUE
    context = get_context()
    
    # On passe 'context' au lieu de 'None'
    unreal.SystemLibrary.execute_console_command(context, f"NetEmulation.PktLag {val}")
    unreal.log(f"Réseau : NetEmulation.PktLag réglé sur {val}ms")
    
    # Actualise le bouton
    main()

def main():
    # LOGIQUE 1 : Seamless Travel
    # On utilise le contexte ici aussi pour éviter l'erreur au boot
    context = get_context()
    unreal.SystemLibrary.execute_console_command(context, "net.AllowPIESeamlessTravel True")
    
    # LOGIQUE 2 : Bouton
    import NetworkLagAndSeamless
    current_lag = NetworkLagAndSeamless.LAG_VALUE

    menus = unreal.ToolMenus.get()
    toolbar = menus.extend_menu("LevelEditor.LevelEditorToolBar.User")
    if not toolbar: return

    entry = unreal.ToolMenuEntry(name="NetworkLagButton", type=unreal.MultiBlockType.MENU_ENTRY)
    entry.set_label(f"Set Lag: {current_lag}ms")
    entry.set_tool_tip(f"Applique {current_lag}ms. Modifiez LAG_VALUE dans le script et sauvegardez.")
    
    entry.set_string_command(
        unreal.ToolMenuStringCommandType.PYTHON,
        "",
        "import NetworkLagAndSeamless; NetworkLagAndSeamless.set_network_lag()"
    )
    
    toolbar.add_menu_entry("Scripts", entry)
    menus.refresh_all_widgets()

if __name__ == "__main__":
    main()