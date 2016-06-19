/* 

Amine Rehioui
Created: December 24th 2012

*/

#ifndef _EXPLOSION_EDITOR_H_INCLUDED_
#define _EXPLOSION_EDITOR_H_INCLUDED_

#include "wx/aui/auibar.h"

#include "EEViewPort.h"
#include "PropertyList.h"
#include "MeshEntity.h"

//! ExplosionEditor main frame
class ExplosionEditor : public wxFrame, public PropertyListListener
{
	typedef wxFrame super;

public:

	//! Constructor
	ExplosionEditor(const wxString& title, const wxSize& size = wxSize(1024, 768));

	// Destructor
	virtual ~ExplosionEditor();

	//! Shows the frame
	void Show();

	//! returns the currently edited mesh
	MeshEntity* GetCurrentMesh();

	//! returns the selected chunk
	inline int GetSelectedChunk() const { return m_SelectedChunk; }

	//! returns the selected SubMesh
	inline int GetSelectedSubMesh() const { return m_SelectedSubMesh; }

	//! sets the selection
	void SetSelection(int chunkIndex, int subMeshIndex);

	//! called when the chunks changed
	void OnChunksChanged();

	//! returns the debug material
	inline Material* GetMaterial() const { return m_DebugMaterial; }

	//! renders a triangle
	void DrawTriangle(const Vector3& v1, const Vector3& v2, const Vector3& v3);

	// event handlers (these functions should _not_ be virtual)
	void OnQuit(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnIdle(wxIdleEvent& event);
	void OnNew(wxCommandEvent& event);
	void OnOpen(wxCommandEvent& event);
	void OnSave(wxCommandEvent& event);
	void OnResetView(wxCommandEvent& event);

	//! call on property change
	void OnPropertyChanged(wxPropertyGridEvent &event);

	//! called while a property is changing
	void OnPropertyChanging(wxPropertyGridEvent &event);

	//! called while a property is selected
	void OnPropertySelected(wxPropertyGridEvent &event);

private:

	//! updates the current mesh entity to edit
	void UpdateCurrentMesh();

	//! validates the chunks in the explosion visitor with respect to the selected mesh
	void ValidateChunks();

	wxAuiManager m_UIManager;

	wxAuiToolBar* m_pToolBar;
	EEViewPort* m_pViewPort;
	PropertyList* m_pPropertyList;

	Reference<Camera> m_Camera;
	Reference<ExplosionVisitor> m_ExplosionVisitor;
	Reference<Material> m_DebugMaterial;
	Reference<VertexBuffer> m_DebugVB;
	uint m_MaxDebugVertices;
	bool m_bChunksChanged;
	
	int m_SelectedChunk;
	int m_SelectedSubMesh;

	DECLARE_EVENT_TABLE()
};

#endif // _EXPLOSION_EDITOR_H_INCLUDED_



