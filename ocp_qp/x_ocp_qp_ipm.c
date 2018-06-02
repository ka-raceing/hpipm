/**************************************************************************************************
*                                                                                                 *
* This file is part of HPIPM.                                                                     *
*                                                                                                 *
* HPIPM -- High Performance Interior Point Method.                                                *
* Copyright (C) 2017 by Gianluca Frison.                                                          *
* Developed at IMTEK (University of Freiburg) under the supervision of Moritz Diehl.              *
* All rights reserved.                                                                            *
*                                                                                                 *
* HPIPM is free software; you can redistribute it and/or                                          *
* modify it under the terms of the GNU Lesser General Public                                      *
* License as published by the Free Software Foundation; either                                    *
* version 2.1 of the License, or (at your option) any later version.                              *
*                                                                                                 *
* HPIPM is distributed in the hope that it will be useful,                                        *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                                  *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                                            *
* See the GNU Lesser General Public License for more details.                                     *
*                                                                                                 *
* You should have received a copy of the GNU Lesser General Public                                *
* License along with HPIPM; if not, write to the Free Software                                    *
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA                  *
*                                                                                                 *
* Author: Gianluca Frison, gianluca.frison (at) imtek.uni-freiburg.de                             *
*                                                                                                 *
**************************************************************************************************/



int MEMSIZE_OCP_QP_IPM_ARG(struct OCP_QP_DIM *dim)
	{

	return 0;

	}



void CREATE_OCP_QP_IPM_ARG(struct OCP_QP_DIM *dim, struct OCP_QP_IPM_ARG *arg, void *mem)
	{

	arg->memsize = 0;

	return;

	}



void SET_DEFAULT_OCP_QP_IPM_ARG(enum OCP_QP_IPM_MODE mode, struct OCP_QP_IPM_ARG *arg)
	{

	if(mode==SPEED)
		{
		arg->mu0 = 1e1;
		arg->alpha_min = 1e-12;
		arg->res_g_max = 1e-6;
		arg->res_b_max = 1e-8;
		arg->res_d_max = 1e-8;
		arg->res_m_max = 1e-8;
		arg->iter_max = 15;
		arg->stat_max = 15;
		arg->pred_corr = 1;
		arg->cond_pred_corr = 1;
		arg->itref_pred_max = 0;
		arg->itref_corr_max = 0;
		arg->reg_prim = 1e-15;
		arg->lq_fact = 0;
		arg->lam_min = 1e-30;
		arg->t_min = 1e-30;
		arg->warm_start = 0;
		}
	else if(mode==BALANCE)
		{
		arg->mu0 = 1e1;
		arg->alpha_min = 1e-12;
		arg->res_g_max = 1e-6;
		arg->res_b_max = 1e-8;
		arg->res_d_max = 1e-8;
		arg->res_m_max = 1e-8;
		arg->iter_max = 30;
		arg->stat_max = 30;
		arg->pred_corr = 1;
		arg->cond_pred_corr = 1;
		arg->itref_pred_max = 0;
		arg->itref_corr_max = 2;
		arg->reg_prim = 1e-15;
		arg->lq_fact = 1;
		arg->lam_min = 1e-30;
		arg->t_min = 1e-30;
		arg->warm_start = 0;
		}
	else if(mode==ROBUST)
		{
		arg->mu0 = 1e2;
		arg->alpha_min = 1e-12;
		arg->res_g_max = 1e-6;
		arg->res_b_max = 1e-8;
		arg->res_d_max = 1e-8;
		arg->res_m_max = 1e-8;
		arg->iter_max = 100;
		arg->stat_max = 100;
		arg->pred_corr = 1;
		arg->cond_pred_corr = 1;
		arg->itref_pred_max = 0;
		arg->itref_corr_max = 4;
		arg->reg_prim = 1e-15;
		arg->lq_fact = 2;
		arg->lam_min = 1e-30;
		arg->t_min = 1e-30;
		arg->warm_start = 0;
		}
	else
		{
		printf("\nwrong set default mode\n");
		exit(1);
		}

	return;

	}



int MEMSIZE_OCP_QP_IPM(struct OCP_QP_DIM *dim, struct OCP_QP_IPM_ARG *arg)
	{

	// loop index
	int ii;

	// extract ocp qp size
	int N = dim->N;
	int *nx = dim->nx;
	int *nu = dim->nu;
	int *nb = dim->nb;
	int *ng = dim->ng;
	int *ns = dim->ns;

	// compute core qp size and max size
	int nvt = 0;
	int net = 0;
	int nct = 0;
	int nxM = 0;
	int nuM = 0;
	int nbM = 0;
	int ngM = 0;
	int nsM = 0;
	for(ii=0; ii<N; ii++)
		{
		nvt += nx[ii]+nu[ii]+2*ns[ii];
		net += nx[ii+1];
		nct += 2*nb[ii]+2*ng[ii]+2*ns[ii];
		nxM = nx[ii]>nxM ? nx[ii] : nxM;
		nuM = nu[ii]>nuM ? nu[ii] : nuM;
		nbM = nb[ii]>nbM ? nb[ii] : nbM;
		ngM = ng[ii]>ngM ? ng[ii] : ngM;
		nsM = ns[ii]>nsM ? ns[ii] : nsM;
		}
	nvt += nx[ii]+nu[ii]+2*ns[ii];
	nct += 2*nb[ii]+2*ng[ii]+2*ns[ii];
	nxM = nx[ii]>nxM ? nx[ii] : nxM;
	nuM = nu[ii]>nuM ? nu[ii] : nuM;
	nbM = nb[ii]>nbM ? nb[ii] : nbM;
	ngM = ng[ii]>ngM ? ng[ii] : ngM;
	nsM = ns[ii]>nsM ? ns[ii] : nsM;

	int size = 0;

	size += 1*sizeof(struct CORE_QP_IPM_WORKSPACE);
	size += 1*MEMSIZE_CORE_QP_IPM(nvt, net, nct);

	size += 1*sizeof(struct OCP_QP_RES_WORKSPACE); // res_workspace

	size += 2*sizeof(struct OCP_QP); // qp_step qp_itref

	size += 2*sizeof(struct OCP_QP_SOL); // sol_step sol_itref
	size += 1*MEMSIZE_OCP_QP_SOL(dim); // sol_itref

	size += 2*sizeof(struct OCP_QP_RES); // res res_itref
	size += 1*MEMSIZE_OCP_QP_RES(dim); // res_itref

	size += 9*(N+1)*sizeof(struct STRVEC); // res_g res_d res_m Gamma gamma Zs_inv sol_step(v,lam,t) 
	size += 3*N*sizeof(struct STRVEC); // res_b Pb sol_step(pi) 
	size += 9*sizeof(struct STRVEC); // tmp_nxM (4+2)*tmp_nbgM (1+1)*tmp_nsM

	size += 1*(N+1)*sizeof(struct STRMAT); // L
	if(arg->lq_fact>0)
		size += 1*(N+1)*sizeof(struct STRMAT); // Lh
	size += 2*sizeof(struct STRMAT); // AL
	if(arg->lq_fact>0)
		size += 1*sizeof(struct STRMAT); // lq0

	size += 1*SIZE_STRVEC(nxM); // tmp_nxM
	size += 4*SIZE_STRVEC(nbM+ngM); // tmp_nbgM
	size += 1*SIZE_STRVEC(nsM); // tmp_nsM
	for(ii=0; ii<N; ii++) size += 1*SIZE_STRVEC(nx[ii+1]); // Pb
	for(ii=0; ii<=N; ii++) size += 1*SIZE_STRVEC(2*ns[ii]); // Zs_inv
	for(ii=0; ii<=N; ii++) size += 2*SIZE_STRMAT(nu[ii]+nx[ii]+1, nu[ii]+nx[ii]); // L
	if(arg->lq_fact>0)
		for(ii=0; ii<=N; ii++) size += 2*SIZE_STRMAT(nu[ii]+nx[ii]+1, nu[ii]+nx[ii]); // Lh
	size += 2*SIZE_STRMAT(nuM+nxM+1, nxM+ngM); // AL
	if(arg->lq_fact>0)
		size += 1*SIZE_STRMAT(nuM+nxM, 2*nuM+3*nxM+ngM); // lq0

	if(arg->lq_fact>0)
		size += 1*GELQF_WORKSIZE(nuM+nxM, 2*nuM+3*nxM+ngM); // lq_work0

	size += 5*arg->stat_max*sizeof(REAL);

	size += (N+1)*sizeof(int); // use_hess_fact

	size = (size+63)/64*64; // make multiple of typical cache line size
	size += 1*64; // align once to typical cache line size

	return size;

	}



void CREATE_OCP_QP_IPM(struct OCP_QP_DIM *dim, struct OCP_QP_IPM_ARG *arg, struct OCP_QP_IPM_WORKSPACE *workspace, void *mem)
	{

	// loop index
	int ii;

	// extract ocp qp size
	int N = dim->N;
	int *nx = dim->nx;
	int *nu = dim->nu;
	int *nb = dim->nb;
	int *ng = dim->ng;
	int *ns = dim->ns;


	// compute core qp size and max size
	int nvt = 0;
	int net = 0;
	int nct = 0;
	int nxM = 0;
	int nuM = 0;
	int nbM = 0;
	int ngM = 0;
	int nsM = 0;
	for(ii=0; ii<N; ii++)
		{
		nvt += nx[ii]+nu[ii]+2*ns[ii];
		net += nx[ii+1];
		nct += 2*nb[ii]+2*ng[ii]+2*ns[ii];
		nxM = nx[ii]>nxM ? nx[ii] : nxM;
		nuM = nu[ii]>nuM ? nu[ii] : nuM;
		nbM = nb[ii]>nbM ? nb[ii] : nbM;
		ngM = ng[ii]>ngM ? ng[ii] : ngM;
		nsM = ns[ii]>nsM ? ns[ii] : nsM;
		}
	nvt += nx[ii]+nu[ii]+2*ns[ii];
	nct += 2*nb[ii]+2*ng[ii]+2*ns[ii];
	nxM = nx[ii]>nxM ? nx[ii] : nxM;
	nuM = nu[ii]>nuM ? nu[ii] : nuM;
	nbM = nb[ii]>nbM ? nb[ii] : nbM;
	ngM = ng[ii]>ngM ? ng[ii] : ngM;
	nsM = ns[ii]>nsM ? ns[ii] : nsM;


	// core struct
	struct CORE_QP_IPM_WORKSPACE *sr_ptr = mem;

	// core workspace
	workspace->core_workspace = sr_ptr;
	sr_ptr += 1;
	struct CORE_QP_IPM_WORKSPACE *cws = workspace->core_workspace;


	// res struct
	struct OCP_QP_RES *res_ptr = (struct OCP_QP_RES *) sr_ptr;
	workspace->res = res_ptr;
	res_ptr += 1;
	workspace->res_itref = res_ptr;
	res_ptr += 1;


	// res workspace struct
	struct OCP_QP_RES_WORKSPACE *res_ws_ptr = (struct OCP_QP_RES_WORKSPACE *) res_ptr;
	workspace->res_workspace = res_ws_ptr;
	res_ws_ptr += 1;


	// qp sol struct
	struct OCP_QP_SOL *qp_sol_ptr = (struct OCP_QP_SOL *) res_ws_ptr;

	workspace->sol_step = qp_sol_ptr;
	qp_sol_ptr += 1;
	workspace->sol_itref = qp_sol_ptr;
	qp_sol_ptr += 1;


	// qp struct
	struct OCP_QP *qp_ptr = (struct OCP_QP *) qp_sol_ptr;

	workspace->qp_step = qp_ptr;
	qp_ptr += 1;
	workspace->qp_itref = qp_ptr;
	qp_ptr += 1;


	// matrix struct
	struct STRMAT *sm_ptr = (struct STRMAT *) qp_ptr;

	workspace->L = sm_ptr;
	sm_ptr += N+1;
	if(arg->lq_fact>0)
		{
		workspace->Lh = sm_ptr;
		sm_ptr += N+1;
		}
	workspace->AL = sm_ptr;
	sm_ptr += 2;
	if(arg->lq_fact>0)
		{
		workspace->lq0 = sm_ptr;
		sm_ptr += 1;
		}


	// vector struct
	struct STRVEC *sv_ptr = (struct STRVEC *) sm_ptr;

	workspace->sol_step->ux = sv_ptr;
	sv_ptr += N+1;
	workspace->sol_step->pi = sv_ptr;
	sv_ptr += N;
	workspace->sol_step->lam = sv_ptr;
	sv_ptr += N+1;
	workspace->sol_step->t = sv_ptr;
	sv_ptr += N+1;
	workspace->res->res_g = sv_ptr;
	sv_ptr += N+1;
	workspace->res->res_b = sv_ptr;
	sv_ptr += N;
	workspace->res->res_d = sv_ptr;
	sv_ptr += N+1;
	workspace->res->res_m = sv_ptr;
	sv_ptr += N+1;
	workspace->Gamma = sv_ptr;
	sv_ptr += N+1;
	workspace->gamma = sv_ptr;
	sv_ptr += N+1;
	workspace->Pb = sv_ptr;
	sv_ptr += N;
	workspace->Zs_inv = sv_ptr;
	sv_ptr += N+1;
	workspace->tmp_nxM = sv_ptr;
	sv_ptr += 1;
	workspace->tmp_nbgM = sv_ptr;
	sv_ptr += 4;
	workspace->res_workspace->tmp_nbgM = sv_ptr;
	sv_ptr += 2;
	workspace->tmp_nsM = sv_ptr;
	sv_ptr += 1;
	workspace->res_workspace->tmp_nsM = sv_ptr;
	sv_ptr += 1;


	// double/float stuff
	REAL *d_ptr = (REAL *) sv_ptr;

	workspace->stat = d_ptr;
	d_ptr += 5*arg->stat_max;

	// int stuff
	int *i_ptr = (int *) d_ptr;

	workspace->use_hess_fact = i_ptr;
	i_ptr += N+1;


	// align to typicl cache line size
	size_t s_ptr = (size_t) i_ptr;
	s_ptr = (s_ptr+63)/64*64;


	// void stuf
	char *c_ptr = (char *) s_ptr;

	CREATE_OCP_QP_SOL(dim, workspace->sol_itref, c_ptr);
	c_ptr += workspace->sol_itref->memsize;

	CREATE_OCP_QP_RES(dim, workspace->res_itref, c_ptr);
	c_ptr += workspace->res_itref->memsize;

	for(ii=0; ii<=N; ii++)
		{
		CREATE_STRMAT(nu[ii]+nx[ii]+1, nu[ii]+nx[ii], workspace->L+ii, c_ptr);
		c_ptr += (workspace->L+ii)->memsize;
		}

	if(arg->lq_fact>0)
		{
		for(ii=0; ii<=N; ii++)
			{
			CREATE_STRMAT(nu[ii]+nx[ii]+1, nu[ii]+nx[ii], workspace->Lh+ii, c_ptr);
			c_ptr += (workspace->Lh+ii)->memsize;
			}
		}

	CREATE_STRMAT(nuM+nxM+1, nxM+ngM, workspace->AL+0, c_ptr);
	c_ptr += (workspace->AL+0)->memsize;

	CREATE_STRMAT(nuM+nxM+1, nxM+ngM, workspace->AL+1, c_ptr);
	c_ptr += (workspace->AL+1)->memsize;

	if(arg->lq_fact>0)
		{
		CREATE_STRMAT(nuM+nxM, 2*nuM+3*nxM+ngM, workspace->lq0, c_ptr);
		c_ptr += (workspace->lq0)->memsize;
		}

	for(ii=0; ii<N; ii++)
		{
		CREATE_STRVEC(nx[ii+1], workspace->Pb+ii, c_ptr);
		c_ptr += (workspace->Pb+ii)->memsize;
		}

	for(ii=0; ii<N+1; ii++)
		{
		CREATE_STRVEC(2*ns[ii], workspace->Zs_inv+ii, c_ptr);
		c_ptr += (workspace->Zs_inv+ii)->memsize;
		}

	CREATE_STRVEC(nxM, workspace->tmp_nxM, c_ptr);
	c_ptr += workspace->tmp_nxM->memsize;

	CREATE_STRVEC(nbM+ngM, workspace->tmp_nbgM+0, c_ptr);
	CREATE_STRVEC(nbM+ngM, workspace->res_workspace->tmp_nbgM+0, c_ptr);
	c_ptr += (workspace->tmp_nbgM+0)->memsize;

	CREATE_STRVEC(nbM+ngM, workspace->tmp_nbgM+1, c_ptr);
	CREATE_STRVEC(nbM+ngM, workspace->res_workspace->tmp_nbgM+1, c_ptr);
	c_ptr += (workspace->tmp_nbgM+1)->memsize;

	CREATE_STRVEC(nbM+ngM, workspace->tmp_nbgM+2, c_ptr);
	c_ptr += (workspace->tmp_nbgM+2)->memsize;

	CREATE_STRVEC(nbM+ngM, workspace->tmp_nbgM+3, c_ptr);
	c_ptr += (workspace->tmp_nbgM+3)->memsize;

	CREATE_STRVEC(nsM, workspace->tmp_nsM+0, c_ptr);
	CREATE_STRVEC(nsM, workspace->res_workspace->tmp_nsM+0, c_ptr);
	c_ptr += (workspace->tmp_nsM+0)->memsize;

	CREATE_CORE_QP_IPM(nvt, net, nct, cws, c_ptr);
	c_ptr += workspace->core_workspace->memsize;

	if(arg->lq_fact>0)
		{
		workspace->lq_work0 = c_ptr;
		c_ptr += GELQF_WORKSIZE(nuM+nxM, 2*nuM+3*nxM+ngM);
		}


	// alias members of workspace and core_workspace
	//
	c_ptr = (char *) cws->dv;
	for(ii=0; ii<=N; ii++)
		{
		CREATE_STRVEC(nu[ii]+nx[ii]+2*ns[ii], workspace->sol_step->ux+ii, c_ptr);
		c_ptr += (nu[ii]+nx[ii])*sizeof(REAL);
		c_ptr += ns[ii]*sizeof(REAL);
		c_ptr += ns[ii]*sizeof(REAL);
		}
	//
	c_ptr = (char *) cws->dpi;
	for(ii=0; ii<N; ii++)
		{
		CREATE_STRVEC(nx[ii+1], workspace->sol_step->pi+ii, c_ptr);
		c_ptr += (nx[ii+1])*sizeof(REAL);
		}
	//
	c_ptr = (char *) cws->dlam;
	for(ii=0; ii<=N; ii++)
		{
		CREATE_STRVEC(2*nb[ii]+2*ng[ii]+2*ns[ii], workspace->sol_step->lam+ii, c_ptr);
		c_ptr += nb[ii]*sizeof(REAL);
		c_ptr += ng[ii]*sizeof(REAL);
		c_ptr += nb[ii]*sizeof(REAL);
		c_ptr += ng[ii]*sizeof(REAL);
		c_ptr += ns[ii]*sizeof(REAL);
		c_ptr += ns[ii]*sizeof(REAL);
		}
	//
	c_ptr = (char *) cws->dt;
	for(ii=0; ii<=N; ii++)
		{
		CREATE_STRVEC(2*nb[ii]+2*ng[ii]+2*ns[ii], workspace->sol_step->t+ii, c_ptr);
		c_ptr += nb[ii]*sizeof(REAL);
		c_ptr += ng[ii]*sizeof(REAL);
		c_ptr += nb[ii]*sizeof(REAL);
		c_ptr += ng[ii]*sizeof(REAL);
		c_ptr += ns[ii]*sizeof(REAL);
		c_ptr += ns[ii]*sizeof(REAL);
		}
	//
	c_ptr = (char *) cws->res_g;
	for(ii=0; ii<=N; ii++)
		{
		CREATE_STRVEC(nu[ii]+nx[ii]+2*ns[ii], workspace->res->res_g+ii, c_ptr);
		c_ptr += (nu[ii]+nx[ii])*sizeof(REAL);
		c_ptr += ns[ii]*sizeof(REAL);
		c_ptr += ns[ii]*sizeof(REAL);
		}
	//
	c_ptr = (char *) cws->res_b;
	for(ii=0; ii<N; ii++)
		{
		CREATE_STRVEC(nx[ii+1], workspace->res->res_b+ii, c_ptr);
		c_ptr += (nx[ii+1])*sizeof(REAL);
		}
	//
	c_ptr = (char *) cws->res_d;
	for(ii=0; ii<=N; ii++)
		{
		CREATE_STRVEC(2*nb[ii]+2*ng[ii]+2*ns[ii], workspace->res->res_d+ii, c_ptr);
		c_ptr += nb[ii]*sizeof(REAL);
		c_ptr += ng[ii]*sizeof(REAL);
		c_ptr += nb[ii]*sizeof(REAL);
		c_ptr += ng[ii]*sizeof(REAL);
		c_ptr += ns[ii]*sizeof(REAL);
		c_ptr += ns[ii]*sizeof(REAL);
		}
	//
	c_ptr = (char *) cws->res_m;
	for(ii=0; ii<=N; ii++)
		{
		CREATE_STRVEC(2*nb[ii]+2*ng[ii]+2*ns[ii], workspace->res->res_m+ii, c_ptr);
		c_ptr += nb[ii]*sizeof(REAL);
		c_ptr += ng[ii]*sizeof(REAL);
		c_ptr += nb[ii]*sizeof(REAL);
		c_ptr += ng[ii]*sizeof(REAL);
		c_ptr += ns[ii]*sizeof(REAL);
		c_ptr += ns[ii]*sizeof(REAL);
		}
	//
	c_ptr = (char *) cws->Gamma;
	for(ii=0; ii<=N; ii++)
		{
		CREATE_STRVEC(2*nb[ii]+2*ng[ii]+2*ns[ii], workspace->Gamma+ii, c_ptr);
		c_ptr += nb[ii]*sizeof(REAL);
		c_ptr += ng[ii]*sizeof(REAL);
		c_ptr += nb[ii]*sizeof(REAL);
		c_ptr += ng[ii]*sizeof(REAL);
		c_ptr += ns[ii]*sizeof(REAL);
		c_ptr += ns[ii]*sizeof(REAL);
		}
	//
	c_ptr = (char *) cws->gamma;
	for(ii=0; ii<=N; ii++)
		{
		CREATE_STRVEC(2*nb[ii]+2*ng[ii]+2*ns[ii], workspace->gamma+ii, c_ptr);
		c_ptr += nb[ii]*sizeof(REAL);
		c_ptr += ng[ii]*sizeof(REAL);
		c_ptr += nb[ii]*sizeof(REAL);
		c_ptr += ng[ii]*sizeof(REAL);
		c_ptr += ns[ii]*sizeof(REAL);
		c_ptr += ns[ii]*sizeof(REAL);
		}


	workspace->res->dim = dim;

	workspace->stat_max = arg->stat_max;

	workspace->warm_start = arg->warm_start;

	for(ii=0; ii<=N; ii++)
		workspace->use_hess_fact[ii] = 0;

	workspace->memsize = MEMSIZE_OCP_QP_IPM(dim, arg);


#if defined(RUNTIME_CHECKS)
	if(c_ptr > ((char *) mem) + workspace->memsize)
		{
		printf("\nCreate_ocp_qp_ipm: outside memory bounds!\n\n");
		exit(1);
		}
#endif


	return;

	}



int SOLVE_OCP_QP_IPM(struct OCP_QP *qp, struct OCP_QP_SOL *qp_sol, struct OCP_QP_IPM_ARG *arg, struct OCP_QP_IPM_WORKSPACE *ws)
	{

#if 0
	int N = qp->dim->N;
	int *nx = qp->dim->nx;
	int *nu = qp->dim->nu;
	int *nb = qp->dim->nb;
	int *ng = qp->dim->ng;
	int *ns = qp->dim->ns;

	int ii;

#if 1
	printf("\nnx\n");
	int_print_mat(1, N+1, nx, 1);
	printf("\nnu\n");
	int_print_mat(1, N+1, nu, 1);
	printf("\nnb\n");
	int_print_mat(1, N+1, nb, 1);
	printf("\nng\n");
	int_print_mat(1, N+1, ng, 1);
	printf("\nns\n");
	int_print_mat(1, N+1, ns, 1);
#endif
	printf("\nBAt\n");
	for(ii=0; ii<N; ii++)
		blasfeo_print_dmat(nu[ii]+nx[ii], nx[ii+1], qp->BAbt+ii, 0, 0);
	printf("\nb\n");
	for(ii=0; ii<N; ii++)
		blasfeo_print_tran_dvec(nx[ii+1], qp->b+ii, 0);
	printf("\nRSQ\n");
	for(ii=0; ii<=N; ii++)
		blasfeo_print_dmat(nu[ii]+nx[ii], nu[ii]+nx[ii], qp->RSQrq+ii, 0, 0);
	printf("\nrq\n");
	for(ii=0; ii<=N; ii++)
		blasfeo_print_tran_dvec(nu[ii]+nx[ii], qp->rqz+ii, 0);
	printf("\nidxb\n");
	for(ii=0; ii<=N; ii++)
		int_print_mat(1, nb[ii], qp->idxb[ii], 1);
	printf("\nDCt\n");
	for(ii=0; ii<=N; ii++)
		blasfeo_print_dmat(nu[ii]+nx[ii], ng[ii], qp->DCt+ii, 0, 0);
	printf("\nd\n");
	for(ii=0; ii<=N; ii++)
		blasfeo_print_tran_dvec(2*nb[ii]+2*ng[ii]+2*ns[ii], qp->d+ii, 0);
#if 0
	printf("\nlb\n");
	for(ii=0; ii<=N; ii++)
		blasfeo_print_tran_dvec(nb[ii], qp->d+ii, 0);
	printf("\nlg\n");
	for(ii=0; ii<=N; ii++)
		blasfeo_print_tran_dvec(ng[ii], qp->d+ii, nb[ii]);
	printf("\nub\n");
	for(ii=0; ii<=N; ii++)
		blasfeo_print_tran_dvec(nb[ii], qp->d+ii, nb[ii]+ng[ii]);
	printf("\nug\n");
	for(ii=0; ii<=N; ii++)
		blasfeo_print_tran_dvec(ng[ii], qp->d+ii, 2*nb[ii]+ng[ii]);
	printf("\nls\n");
	for(ii=0; ii<=N; ii++)
		blasfeo_print_tran_dvec(ns[ii], qp->d+ii, 2*nb[ii]+2*ng[ii]);
	printf("\nus\n");
	for(ii=0; ii<=N; ii++)
		blasfeo_print_tran_dvec(ns[ii], qp->d+ii, 2*nb[ii]+2*ng[ii]+ns[ii]);
	printf("\nidxb\n");
	for(ii=0; ii<=N; ii++)
		int_print_mat(1, nb[ii], qp->idxb[ii], 1);
	printf("\nidxs\n");
	for(ii=0; ii<=N; ii++)
		int_print_mat(1, ns[ii], qp->idxs[ii], 1);
	printf("\nZ\n");
	for(ii=0; ii<=N; ii++)
		blasfeo_print_tran_dvec(2*ns[ii], qp->Z+ii, 0);
	printf("\nrqz\n");
	for(ii=0; ii<=N; ii++)
		blasfeo_print_tran_dvec(nu[ii]+nx[ii]+2*ns[ii], qp->rqz+ii, 0);
#endif
#endif

	struct CORE_QP_IPM_WORKSPACE *cws = ws->core_workspace;

	// arg to core workspace
	cws->lam_min = arg->lam_min;
	cws->t_min = arg->t_min;

	// alias qp vectors into qp_sol
	cws->v = qp_sol->ux->pa;
	cws->pi = qp_sol->pi->pa;
	cws->lam = qp_sol->lam->pa;
	cws->t = qp_sol->t->pa;

	// alias members of qp_step
	ws->qp_step->dim = qp->dim;
	ws->qp_step->RSQrq = qp->RSQrq;
	ws->qp_step->BAbt = qp->BAbt;
	ws->qp_step->DCt = qp->DCt;
	ws->qp_step->Z = qp->Z;
	ws->qp_step->idxb = qp->idxb;
	ws->qp_step->idxs = qp->idxs;
	ws->qp_step->rqz = ws->res->res_g;
	ws->qp_step->b = ws->res->res_b;
	ws->qp_step->d = ws->res->res_d;
	ws->qp_step->m = ws->res->res_m;

	// alias members of qp_step
	ws->qp_itref->dim = qp->dim;
	ws->qp_itref->RSQrq = qp->RSQrq;
	ws->qp_itref->BAbt = qp->BAbt;
	ws->qp_itref->DCt = qp->DCt;
	ws->qp_itref->Z = qp->Z;
	ws->qp_itref->idxb = qp->idxb;
	ws->qp_itref->idxs = qp->idxs;
	ws->qp_itref->rqz = ws->res_itref->res_g;
	ws->qp_itref->b = ws->res_itref->res_b;
	ws->qp_itref->d = ws->res_itref->res_d;
	ws->qp_itref->m = ws->res_itref->res_m;

	// alias members of qp_itref

	// no constraints
	if(cws->nc==0)
		{
		FACT_SOLVE_KKT_UNCONSTR_OCP_QP(qp, qp_sol, arg, ws);
		COMPUTE_RES_OCP_QP(qp, qp_sol, ws->res, ws->res_workspace);
		cws->mu = ws->res->res_mu;
		ws->iter = 0;
		return 0;
		}

	// blasfeo alias for residuals
	struct STRVEC str_res_g;
	struct STRVEC str_res_b;
	struct STRVEC str_res_d;
	struct STRVEC str_res_m;
	str_res_g.m = cws->nv;
	str_res_b.m = cws->ne;
	str_res_d.m = cws->nc;
	str_res_m.m = cws->nc;
	str_res_g.pa = cws->res_g;
	str_res_b.pa = cws->res_b;
	str_res_d.pa = cws->res_d;
	str_res_m.pa = cws->res_m;

	REAL *qp_res = ws->qp_res;
	qp_res[0] = 0;
	qp_res[1] = 0;
	qp_res[2] = 0;
	qp_res[3] = 0;

	ws->mu0 = arg->mu0;

	int N = qp->dim->N;
	int *nx = qp->dim->nx;
	int *nu = qp->dim->nu;
	int *nb = qp->dim->nb;
	int *ng = qp->dim->ng;
	int *ns = qp->dim->ns;

	int kk, ii, itref0=0, itref1=0, iter_ref_step;
	REAL tmp;
	REAL mu_aff0;

	// init solver
	INIT_VAR_OCP_QP(qp, qp_sol, ws);

	// compute residuals
	COMPUTE_RES_OCP_QP(qp, qp_sol, ws->res, ws->res_workspace);
	BACKUP_RES_M(cws);
	cws->mu = ws->res->res_mu;

	cws->alpha = 1.0;

	// compute infinity norm of residuals
	VECNRM_INF_LIBSTR(cws->nv, &str_res_g, 0, &qp_res[0]);
	VECNRM_INF_LIBSTR(cws->ne, &str_res_b, 0, &qp_res[1]);
	VECNRM_INF_LIBSTR(cws->nc, &str_res_d, 0, &qp_res[2]);
	VECNRM_INF_LIBSTR(cws->nc, &str_res_m, 0, &qp_res[3]);

	REAL itref_qp_norm[4] = {0,0,0,0};
	REAL itref_qp_norm0[4] = {0,0,0,0};
	int ndp0, ndp1;

	int force_lq = 0;


	// IPM loop
	for(kk=0; kk<arg->iter_max & \
			cws->alpha>arg->alpha_min & \
			(qp_res[0]>arg->res_g_max | \
			qp_res[1]>arg->res_b_max | \
			qp_res[2]>arg->res_d_max | \
			qp_res[3]>arg->res_m_max); kk++)
		{

		// fact and solve kkt
		if(arg->lq_fact==0)
			{

			// syrk+cholesky
			FACT_SOLVE_KKT_STEP_OCP_QP(ws->qp_step, ws->sol_step, arg, ws);

			}
		else if(arg->lq_fact==1 & force_lq==0)
			{

			// syrk+chol, switch to lq when needed
			FACT_SOLVE_KKT_STEP_OCP_QP(ws->qp_step, ws->sol_step, arg, ws);

			// compute res of linear system
			COMPUTE_LIN_RES_OCP_QP(ws->qp_step, qp_sol, ws->sol_step, ws->res_itref, ws->res_workspace);
			VECNRM_INF_LIBSTR(cws->nv, ws->res_itref->res_g, 0, &itref_qp_norm[0]);
			VECNRM_INF_LIBSTR(cws->ne, ws->res_itref->res_b, 0, &itref_qp_norm[1]);
			VECNRM_INF_LIBSTR(cws->nc, ws->res_itref->res_d, 0, &itref_qp_norm[2]);
			VECNRM_INF_LIBSTR(cws->nc, ws->res_itref->res_m, 0, &itref_qp_norm[3]);

//printf("\n%e\t%e\t%e\t%e\n", itref_qp_norm[0], itref_qp_norm[1], itref_qp_norm[2], itref_qp_norm[3]);

			// inaccurate factorization: switch to lq
			if(
#ifdef USE_C99_MATH
				( itref_qp_norm[0]==0.0 & isnan(BLASFEO_DVECEL(ws->res_itref->res_g+0, 0)) ) |
#else
				( itref_qp_norm[0]==0.0 & BLASFEO_DVECEL(ws->res_itref->res_g+0, 0)!=BLASFEO_DVECEL(ws->res_itref->res_g+0, 0) ) |
#endif
				itref_qp_norm[0]>1e-5 |
				itref_qp_norm[1]>1e-5 |
				itref_qp_norm[2]>1e-5 |
				itref_qp_norm[3]>1e-5 )
				{

#if 0
blasfeo_print_tran_dvec(cws->nv, ws->sol_step->v, 0);
blasfeo_print_tran_dvec(cws->ne, ws->sol_step->pi, 0);
blasfeo_print_tran_dvec(cws->nc, ws->sol_step->lam, 0);
blasfeo_print_tran_dvec(cws->nc, ws->sol_step->t, 0);
#endif

				// refactorize using lq
				FACT_LQ_SOLVE_KKT_STEP_OCP_QP(ws->qp_step, ws->sol_step, arg, ws);

				// switch to lq
				force_lq = 1;

#if 0
blasfeo_print_tran_dvec(cws->nv, ws->sol_step->v, 0);
blasfeo_print_tran_dvec(cws->ne, ws->sol_step->pi, 0);
blasfeo_print_tran_dvec(cws->nc, ws->sol_step->lam, 0);
blasfeo_print_tran_dvec(cws->nc, ws->sol_step->t, 0);
#endif

				}

			}
		else // arg->lq_fact==2
			{

			FACT_LQ_SOLVE_KKT_STEP_OCP_QP(ws->qp_step, ws->sol_step, arg, ws);

			}

		// iterative refinement on prediction step
		for(itref0=0; itref0<arg->itref_pred_max; itref0++)
			{

			COMPUTE_LIN_RES_OCP_QP(ws->qp_step, qp_sol, ws->sol_step, ws->res_itref, ws->res_workspace);

			VECNRM_INF_LIBSTR(cws->nv, ws->res_itref->res_g, 0, &itref_qp_norm[0]);
			VECNRM_INF_LIBSTR(cws->ne, ws->res_itref->res_b, 0, &itref_qp_norm[1]);
			VECNRM_INF_LIBSTR(cws->nc, ws->res_itref->res_d, 0, &itref_qp_norm[2]);
			VECNRM_INF_LIBSTR(cws->nc, ws->res_itref->res_m, 0, &itref_qp_norm[3]);

			if(itref0==0)
				{
				itref_qp_norm0[0] = itref_qp_norm[0];
				itref_qp_norm0[1] = itref_qp_norm[1];
				itref_qp_norm0[2] = itref_qp_norm[2];
				itref_qp_norm0[3] = itref_qp_norm[3];
				}

			if( \
					(itref_qp_norm[0]<1e0*arg->res_g_max | itref_qp_norm[0]<1e-3*qp_res[0]) & \
					(itref_qp_norm[1]<1e0*arg->res_b_max | itref_qp_norm[1]<1e-3*qp_res[1]) & \
					(itref_qp_norm[2]<1e0*arg->res_d_max | itref_qp_norm[2]<1e-3*qp_res[2]) & \
					(itref_qp_norm[3]<1e0*arg->res_m_max | itref_qp_norm[3]<1e-3*qp_res[3]) )
//					(itref_qp_norm[0]<=arg->res_g_max) & \
					(itref_qp_norm[1]<=arg->res_b_max) & \
					(itref_qp_norm[2]<=arg->res_d_max) & \
					(itref_qp_norm[3]<=arg->res_m_max) )
				{
				break;
				}

			SOLVE_KKT_STEP_OCP_QP(ws->qp_itref, ws->sol_itref, arg, ws);

			for(ii=0; ii<=N; ii++)
				AXPY_LIBSTR(nu[ii]+nx[ii]+2*ns[ii], 1.0, ws->sol_itref->ux+ii, 0, ws->sol_step->ux+ii, 0, ws->sol_step->ux+ii, 0);
			for(ii=0; ii<N; ii++)
				AXPY_LIBSTR(nx[ii+1], 1.0, ws->sol_itref->pi+ii, 0, ws->sol_step->pi+ii, 0, ws->sol_step->pi+ii, 0);
			for(ii=0; ii<=N; ii++)
				AXPY_LIBSTR(2*nb[ii]+2*ng[ii]+2*ns[ii], 1.0, ws->sol_itref->lam+ii, 0, ws->sol_step->lam+ii, 0, ws->sol_step->lam+ii, 0);
			for(ii=0; ii<=N; ii++)
				AXPY_LIBSTR(2*nb[ii]+2*ng[ii]+2*ns[ii], 1.0, ws->sol_itref->t+ii, 0, ws->sol_step->t+ii, 0, ws->sol_step->t+ii, 0);

			}

#if 0
int N = qp->dim->N;
int *nx = qp->dim->nx;
int *nu = qp->dim->nu;
int *nb = qp->dim->nb;
int *ng = qp->dim->ng;
int *ns = qp->dim->ns;

int ii;

//exit(1);

//for(ii=0; ii<=N; ii++)
//	blasfeo_print_dmat(nu[ii]+nx[ii], nu[ii]+nx[ii], ws->L+ii, 0, 0);
//exit(1);

printf("\nux\n");
for(ii=0; ii<=N; ii++)
	blasfeo_print_tran_dvec(nu[ii]+nx[ii]+2*ns[ii], ws->dux+ii, 0);
printf("\npi\n");
for(ii=0; ii<N; ii++)
	blasfeo_print_tran_dvec(nx[ii+1], ws->dpi+ii, 0);
//printf("\nlam\n");
//for(ii=0; ii<=N; ii++)
//	blasfeo_print_tran_dvec(2*nb[ii]+2*ng[ii]+2*ns[ii], ws->dlam+ii, 0);
printf("\nt\n");
for(ii=0; ii<=N; ii++)
	blasfeo_print_tran_dvec(2*nb[ii]+2*ng[ii]+2*ns[ii], ws->dt+ii, 0);

SOLVE_KKT_STEP_OCP_QP(qp, ws);

printf("\nux\n");
for(ii=0; ii<=N; ii++)
	blasfeo_print_tran_dvec(nu[ii]+nx[ii]+2*ns[ii], ws->dux+ii, 0);
printf("\npi\n");
for(ii=0; ii<N; ii++)
	blasfeo_print_tran_dvec(nx[ii+1], ws->dpi+ii, 0);
//printf("\nlam\n");
//for(ii=0; ii<=N; ii++)
//	blasfeo_print_tran_dvec(2*nb[ii]+2*ng[ii]+2*ns[ii], ws->dlam+ii, 0);
printf("\nt\n");
for(ii=0; ii<=N; ii++)
	blasfeo_print_tran_dvec(2*nb[ii]+2*ng[ii]+2*ns[ii], ws->dt+ii, 0);

exit(1);
#endif

		// alpha
		COMPUTE_ALPHA_QP(cws);
		if(kk<ws->stat_max)
			ws->stat[5*kk+0] = cws->alpha;

		// Mehrotra's predictor-corrector
		if(arg->pred_corr==1)
			{
			// mu_aff
			COMPUTE_MU_AFF_QP(cws);
			if(kk<ws->stat_max)
				ws->stat[5*kk+1] = cws->mu_aff;

			tmp = cws->mu_aff/cws->mu;
			cws->sigma = tmp*tmp*tmp;
			if(kk<ws->stat_max)
				ws->stat[5*kk+2] = cws->sigma;

			COMPUTE_CENTERING_CORRECTION_QP(cws);

			// fact and solve kkt
			SOLVE_KKT_STEP_OCP_QP(ws->qp_step, ws->sol_step, arg, ws);

			// alpha
			COMPUTE_ALPHA_QP(cws);
			if(kk<ws->stat_max)
				ws->stat[5*kk+3] = cws->alpha;

			// conditional Mehrotra's predictor-corrector
			if(arg->cond_pred_corr==1)
				{

				// save mu_aff (from prediction step)
				mu_aff0 = cws->mu_aff;

				// compute mu for predictor-corrector-centering
				COMPUTE_MU_AFF_QP(cws);

//				if(cws->mu_aff > 2.0*cws->mu)
				if(cws->mu_aff > 2.0*mu_aff0)
					{

					// centering direction
					COMPUTE_CENTERING_QP(cws);

					// solve kkt
					SOLVE_KKT_STEP_OCP_QP(ws->qp_step, ws->sol_step, arg, ws);

					// alpha
					COMPUTE_ALPHA_QP(cws);
					if(kk<ws->stat_max)
						ws->stat[5*kk+3] = cws->alpha;

					}

				}

			iter_ref_step = 0;
			for(itref1=0; itref1<arg->itref_corr_max; itref1++)
				{

				COMPUTE_LIN_RES_OCP_QP(ws->qp_step, qp_sol, ws->sol_step, ws->res_itref, ws->res_workspace);

//for(ii=0; ii<=N; ii++)
//	blasfeo_dvecse(nu[ii]+nx[ii], 0.0, ws->res_itref->res_g+ii, 0);
//for(ii=0; ii<N; ii++)
//	blasfeo_dvecse(nx[ii+1], 0.0, ws->res_itref->res_b+ii, 0);
//for(ii=0; ii<=N; ii++)
//	blasfeo_dvecse(2*nb[ii]+2*ng[ii], 0.0, ws->res_itref->res_d+ii, 0);
//for(ii=0; ii<=N; ii++)
//	blasfeo_dvecse(2*nb[ii]+2*ng[ii], 0.0, ws->res_itref->res_m+ii, 0);
				VECNRM_INF_LIBSTR(cws->nv, ws->res_itref->res_g, 0, &itref_qp_norm[0]);
				VECNRM_INF_LIBSTR(cws->ne, ws->res_itref->res_b, 0, &itref_qp_norm[1]);
				VECNRM_INF_LIBSTR(cws->nc, ws->res_itref->res_d, 0, &itref_qp_norm[2]);
				VECNRM_INF_LIBSTR(cws->nc, ws->res_itref->res_m, 0, &itref_qp_norm[3]);

				if(itref1==0)
					{
					itref_qp_norm0[0] = itref_qp_norm[0];
					itref_qp_norm0[1] = itref_qp_norm[1];
					itref_qp_norm0[2] = itref_qp_norm[2];
					itref_qp_norm0[3] = itref_qp_norm[3];
					}

//printf("\nitref1 %d\t%e\t%e\t%e\t%e\n", itref1, itref_qp_norm[0], itref_qp_norm[1], itref_qp_norm[2], itref_qp_norm[3]);

				if( \
						(itref_qp_norm[0]<1e0*arg->res_g_max | itref_qp_norm[0]<1e-3*qp_res[0]) & \
						(itref_qp_norm[1]<1e0*arg->res_b_max | itref_qp_norm[1]<1e-3*qp_res[1]) & \
						(itref_qp_norm[2]<1e0*arg->res_d_max | itref_qp_norm[2]<1e-3*qp_res[2]) & \
						(itref_qp_norm[3]<1e0*arg->res_m_max | itref_qp_norm[3]<1e-3*qp_res[3]) )
//						(itref_qp_norm[0]<=arg->res_g_max) & \
						(itref_qp_norm[1]<=arg->res_b_max) & \
						(itref_qp_norm[2]<=arg->res_d_max) & \
						(itref_qp_norm[3]<=arg->res_m_max) )
					{
					break;
					}

//printf("\nres_g\n");
//ii = 0;
//blasfeo_print_exp_tran_dvec(nu[ii]+nx[ii], ws->res_itref->res_g+ii, 0);
//for(ii=0; ii<=N; ii++)
//	blasfeo_print_exp_tran_dvec(nu[ii]+nx[ii], ws->res_itref->res_g+ii, 0);
//printf("\nres_b\n");
//for(ii=0; ii<N; ii++)
//	blasfeo_print_exp_tran_dvec(nx[ii+1], ws->res_itref->res_b+ii, 0);
//printf("\nres_d\n");
//for(ii=0; ii<=N; ii++)
//	blasfeo_print_exp_tran_dvec(2*nb[ii]+2*ng[ii], ws->res_itref->res_d+ii, 0);
//printf("\nres_m\n");
//for(ii=0; ii<=N; ii++)
//	blasfeo_print_exp_tran_dvec(2*nb[ii]+2*ng[ii], ws->res_itref->res_m+ii, 0);

				SOLVE_KKT_STEP_OCP_QP(ws->qp_itref, ws->sol_itref, arg, ws);
//				FACT_SOLVE_LQ_KKT_STEP_OCP_QP(ws->qp_itref, ws->sol_itref, arg, ws);
				iter_ref_step = 1;

//printf("\nux_corr\n");
//ii = 0;
//blasfeo_print_exp_tran_dvec(nu[ii]+nx[ii], ws->sol_itref->ux+ii, 0);
//for(ii=0; ii<=N; ii++)
//	blasfeo_print_exp_tran_dvec(nu[ii]+nx[ii], ws->sol_itref->ux+ii, 0);
//printf("\npi_corr\n");
//for(ii=0; ii<N; ii++)
//	blasfeo_print_exp_tran_dvec(nx[ii+1], ws->sol_itref->pi+ii, 0);
//printf("\nlam_corr\n");
//for(ii=0; ii<=N; ii++)
//	blasfeo_print_exp_tran_dvec(2*nb[ii]+2*ng[ii], ws->sol_itref->lam+ii, 0);
//printf("\nt_corr\n");
//for(ii=0; ii<=N; ii++)
//	blasfeo_print_exp_tran_dvec(2*nb[ii]+2*ng[ii], ws->sol_itref->t+ii, 0);

				for(ii=0; ii<=N; ii++)
					AXPY_LIBSTR(nu[ii]+nx[ii]+2*ns[ii], 1.0, ws->sol_itref->ux+ii, 0, ws->sol_step->ux+ii, 0, ws->sol_step->ux+ii, 0);
				for(ii=0; ii<N; ii++)
					AXPY_LIBSTR(nx[ii+1], 1.0, ws->sol_itref->pi+ii, 0, ws->sol_step->pi+ii, 0, ws->sol_step->pi+ii, 0);
				for(ii=0; ii<=N; ii++)
					AXPY_LIBSTR(2*nb[ii]+2*ng[ii]+2*ns[ii], 1.0, ws->sol_itref->lam+ii, 0, ws->sol_step->lam+ii, 0, ws->sol_step->lam+ii, 0);
				for(ii=0; ii<=N; ii++)
					AXPY_LIBSTR(2*nb[ii]+2*ng[ii]+2*ns[ii], 1.0, ws->sol_itref->t+ii, 0, ws->sol_step->t+ii, 0, ws->sol_step->t+ii, 0);

				}

			if(iter_ref_step)
				{
				// alpha
				COMPUTE_ALPHA_QP(cws);
				if(kk<ws->stat_max)
					ws->stat[5*kk+3] = cws->alpha;
				}

			}

		//
		UPDATE_VAR_QP(cws);

		// compute residuals
		COMPUTE_RES_OCP_QP(qp, qp_sol, ws->res, ws->res_workspace);
		BACKUP_RES_M(cws);
		cws->mu = ws->res->res_mu;
		if(kk<ws->stat_max)
			ws->stat[5*kk+4] = ws->res->res_mu;

		// compute infinity norm of residuals
		VECNRM_INF_LIBSTR(cws->nv, &str_res_g, 0, &qp_res[0]);
		VECNRM_INF_LIBSTR(cws->ne, &str_res_b, 0, &qp_res[1]);
		VECNRM_INF_LIBSTR(cws->nc, &str_res_d, 0, &qp_res[2]);
		VECNRM_INF_LIBSTR(cws->nc, &str_res_m, 0, &qp_res[3]);

//printf("\niter %d\t%e\t%e\t%e\t%e\n", kk, qp_res[0], qp_res[1], qp_res[2], qp_res[3]);

		}

	ws->iter = kk;

#if 0
	printf("\nux\n");
	for(ii=0; ii<=N; ii++)
		blasfeo_print_tran_dvec(nu[ii]+nx[ii], qp_sol->ux+ii, 0);
#endif

	// max iteration number reached
	if(kk == arg->iter_max)
		return 1;

	// min step lenght
	if(cws->alpha <= arg->alpha_min)
		return 2;

	// NaN in the solution
#ifdef USE_C99_MATH
	if(isnan(cws->mu))
		return 3;
#else
	if(cws->mu != cws->mu)
		return 3;
#endif

	// normal return
	return 0;

	}



